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

#define TOLERANCE (50)

typedef struct{
    int i;
    int j;
} vec2;

/*
void foo(int* buf, Mat* oldData, int new_color, int p_i, int p_j, int w, int h){
    vector<vec2> processingList;
    processingList.push_back((vec2){p_i,p_j});

    int old_color = oldData.data[i * w + j];
    if (abs(old_color - new_color) < TOLERANCE){
        return;
    }

    for(int count = 0; count < processingList.size(); count++){
        int i = processingList[count].i;
        int j = processingList[count].j;
        int old_color = oldData.data[i * w + j];
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
                            processingList.push_back((vec2){ni,nj});
                        }
                    }
                }
            }
        }
    }
}
*/

void navigate(int* newData, Mat* oldData, int p_i, int p_j, int w, int h){
    vector<vec2> processingList;
    processingList.push_back((vec2){p_i,p_j});

    //until tree exhausted
    for (int k = 0; k < processingList.size(); k++){
        //if this point's tree not yet assigned
        int i = processingList[k].i;
        int j = processingList[k].j;
        if (newData[i * w + j] == -1){
            //if this belongs to the tree then process it and it's neighbors
            //if similar enough to root node
            int this_color = oldData->data[i * w + j];
            int orig_color = oldData->data[p_i * w + p_j];
            if (abs(this_color - orig_color) < TOLERANCE){
                newData[i * w + j] = oldData->data[(p_i * w + p_j)];

                for (int ii = -1; ii <= 1; ii++){
                    for (int jj = -1; jj <= 1; jj++){
                        if (!ii != !jj){
                            int n_i = ii + i;
                            int n_j = jj + j;
                            if (n_i >= 0 && n_j >= 0 && n_i < h && n_j < w){
                                processingList.push_back((vec2){n_i,n_j});
                            }
                        }
                    }
                }
            }
        }
    }
}

void doClusters(){
    image = &original_image;

    int x = image->step;
    int y = image->rows;
    int* clusterNums = new int[x * y];

    for (int i = 0; i < y; i++){
        for (int j = 0; j < x; j++){
            //clusterNums[(i * x) + j] = original_image.data[(i * x) + j];
            clusterNums[(i * x) + j] = -1;
        }
    }

    for (int i = 0; i < y; i++){
        for (int j = 0; j < x; j++){
            navigate(clusterNums, image, i, j, x, y);
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
