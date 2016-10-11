#include <unistd.h>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

#include "pixel.h"

using namespace std;
using namespace cv;

int main(int argc, char **argv) {
    if(argc != 2) {
        cout << "USAGE: skeleton <input file path>" << endl;
        return -1;
    }

    //Load two copies of the image. One to leave as the original, and one to be modified.
    //Done for display purposes only
    //Use CV_LOAD_IMAGE_GRAYSCALE for greyscale images
    Mat original_image = imread(argv[1], CV_LOAD_IMAGE_COLOR);
    Mat modified_image = imread(argv[1], CV_LOAD_IMAGE_COLOR);

    //Create a pointer so that we can quickly toggle between which image is being displayed
    Mat *image = &original_image;

    //Check that the images loaded
    if(!original_image.data || !modified_image.data) {
        cout << "ERROR: Could not load image data." << endl;
        return -1;
    }

    //Create the display window
    namedWindow("Unix Sample Skeleton");

    double blur_list[] = { 1 / 16.0, 2 / 16.0, 1 / 16.0,
                           2 / 16.0, 4 / 16.0, 2 / 16.0,
                           1 / 16.0, 2 / 16.0, 1 / 16.0 };
    mask blur(3, sizeof(blur_list) / sizeof(double), blur_list);

    imageGrid img(modified_image.rows, modified_image.step / 3, &modified_image.data[0]);
    img.multiply(blur);
    img.commitImageGrid(&modified_image.data[0]);

    //Display loop
    bool loop = true;
    while(loop) {
        imshow("Unix Sample Skeleton", *image);
    
        switch(cvWaitKey(15)) {
            case 27:  //Exit display loop if ESC is pressed
                loop = false;
            break;
            case 32:  //Swap image pointer if space is pressed
                if(image == &original_image) {
                  image = &modified_image;
                } else {
                  image = &original_image;
                }
            break;
            default:
            break;
        }
    }
}
