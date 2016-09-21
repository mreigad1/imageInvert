#include <unistd.h>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

#include <fstream>

#include <vector>

using namespace std;
using namespace cv;

#define BUCKET_SIZE 1
#define COLOR_BYTES 1
#define COLOR_RANGE ((1 << (COLOR_BYTES * 8)) - 1)
#define NUM_BUCKETS (COLOR_RANGE / BUCKET_SIZE)

#define INF (~0)

Mat original_image;
Mat modified_image;
Mat eq_image;
Mat eqcv_image;
Mat clust_image;

Mat *image = &original_image;

//unsigned char* data;

#define TOLERANCE 10

void navigate(int* buf, int old_color, int new_color, int i, int j, int w, int h){
	if (abs(old_color - new_color) < TOLERANCE){
		return;
	}
	if (abs(buf[i * w + j] - old_color) < TOLERANCE){
		buf[i * w + j] = new_color;
		for (int ti = -1; ti <= 1; ti ++){
			for (int tj = -1; tj <= 1; tj++){
				if (!ti != !tj){ //exclusive logical or
					int ni = i + ti;
					int nj = j + tj;
					if (ni >= 0 && ni < h && nj >= 0 && nj < w){
						navigate(buf, old_color, new_color, ni, nj, w, h);
					}
				}
			}
		}
	}
}

void doClusters(){
    int x = image->step;
    int y = image->rows;
    unsigned char* data = original_image.data;
    int* clusterNums = new int[x * y];

    for (int i = 0; i < y; i++){
        for (int j = 0; j < x; j++){
            clusterNums[(i * x) + j] = data[(i * x) + j];
        }
    }

    for (int i = 0; i < y; i++){
        for (int j = 0; j < x; j++){
            navigate(clusterNums, clusterNums[i * x + j], (i * x + j), i, j, x, y);
        }
    }

    for (int i = 0; i < y; i++){
        for (int j = 0; j < x; j++){
            clust_image.data[i * x + j] = clusterNums[i * x + j] % 256;
        }
    }

    image = &clust_image;

    delete [] clusterNums;
}

int BHThreshold(int i_s, int i_e, unsigned int* histogram) {

    unsigned int sum = 0;
    for(int i = i_s; i < i_e; i++){
        sum += histogram[i] * i;
    }

    unsigned sum2 = 0;
    int result = i_s;
    for(int i = i_s; sum2 < sum / 2 && i < i_e; i++){
        sum2 += histogram[i] * i;
        result = i;
    }

    return result;
}

int main(int argc, char **argv) {
    
    if (argc != 2) {
        cout << "USAGE: skeleton <input file path>" << endl;
        return -1;
    }
    
    string img_filename = string(argv[1]) + ".bmp";
    string csv_filename = string(argv[1]) + ".csv";

    //Load two copies of the image. One to leave as the original, and one to be modified.
    //Done for display purposes only
    //Use CV_LOAD_IMAGE_GRAYSCALE for greyscale images
    original_image = imread(img_filename.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
    modified_image = imread(img_filename.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
    eq_image = imread(img_filename.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
    clust_image = imread(img_filename.c_str(), CV_LOAD_IMAGE_GRAYSCALE);

    equalizeHist(original_image, eqcv_image);

    //Create a pointer so that we can quickly toggle between which image is being displayed
    image = &original_image;
    
    //Check that the images loaded
    if (!original_image.data || !modified_image.data) {
        cout << "ERROR: Could not load image data." << endl;
        return -1;
    }
    
    //Create the display window
    namedWindow("Unix Sample Skeleton");
    
    //Replace center third of the image with white
    //This can be replaced with whatever filtering you need to do.

    unsigned int histogram_orig[NUM_BUCKETS];
    unsigned int histogram_mod[NUM_BUCKETS];
    unsigned int histogram_eq[NUM_BUCKETS];
    
    for (int i = 0; i <= NUM_BUCKETS; i++) {
        histogram_orig[i] = histogram_mod[i] = 0;
    }
    
    for (size_t i = 0; i < image->rows * image->step; i++) {
        modified_image.data[i] = COLOR_RANGE - modified_image.data[i];
        histogram_orig[(original_image.data[i] / BUCKET_SIZE) % (COLOR_RANGE + 1)]++;
        histogram_mod[(modified_image.data[i] / BUCKET_SIZE) % (COLOR_RANGE + 1)]++;
    }

    int bal_pt;

    cout << "Optimal S value for balancing = " << (bal_pt = BHThreshold(0, NUM_BUCKETS, &histogram_orig[0])) << endl;

    for (size_t i = 0; i < image->rows * image->step; i++) {
        double t = eq_image.data[i];
        if (t < bal_pt){
            t = (t * 1.0 / bal_pt) * (COLOR_RANGE / 2.0);
        } else {
            t = (((t - bal_pt) / (COLOR_RANGE - bal_pt)) * (COLOR_RANGE / 2.0)) + (COLOR_RANGE / 2.0);
        }
        eq_image.data[i] = t;    
    }

    for (size_t i = 0; i < image->rows * image->step; i++) {
        histogram_eq[(eq_image.data[i] / BUCKET_SIZE) % (COLOR_RANGE + 1)]++;
    }

    ofstream histogram;
    histogram.open(csv_filename.c_str());
    histogram << "Intensity,Original,Negative,Equalized\n";
    for (int i = 0; i <= NUM_BUCKETS; i++){
        histogram << i << "," << histogram_orig[i] << "," << histogram_mod[i] << "," << histogram_eq[i] << "\n";
    }
    histogram.close();

    //Display loop
    bool loop = true;
    while (loop) {
        imshow("Unix Sample Skeleton", *image);
        
        switch (cvWaitKey(15)) {
            case 'X':
                sleep(5);
            case 27:  //Exit display loop if ESC is pressed
                //loop = false;
                exit(0);
            break;
            case 'm':  //Swap image pointer if space is pressed
                image = &modified_image;
            break;
            case 'o':
                image = &original_image;
            break;
            case 'e':
                image = &eq_image;
            break;
            case 'c':
                image = &eqcv_image;
            break;
            case 't':
            	doClusters();
            break;
            default:
            break;
        }
    }
}
