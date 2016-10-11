#include <assert.h>

#include "pixel.h"

#define MAX(a,b) ((a > b)? a : b)

enum BGR{
	RGB_B = 0,
	RGB_G = 1,
	RGB_R = 2
};

pixel::pixel(unsigned char R, unsigned char G, unsigned char B ) {
	rgb[RGB_R] = R;
	rgb[RGB_G] = G;
	rgb[RGB_B] = B;
}

pixel::pixel() {
	rgb[RGB_R] = 0;
	rgb[RGB_G] = 0;
	rgb[RGB_B] = 0;
}

pixel pixel::operator+(const pixel& neighbor){
	unsigned int R = rgb[RGB_R] + neighbor.rgb[RGB_R];
	unsigned int G = rgb[RGB_G] + neighbor.rgb[RGB_G];
	unsigned int B = rgb[RGB_B] + neighbor.rgb[RGB_B];
	return pixel((unsigned char)R, (unsigned char)G, (unsigned char)B);
}

pixel pixel::operator-(const pixel& neighbor){
	unsigned int R = rgb[RGB_R] - neighbor.rgb[RGB_R];
	unsigned int G = rgb[RGB_G] - neighbor.rgb[RGB_G];
	unsigned int B = rgb[RGB_B] - neighbor.rgb[RGB_B];
	return pixel((unsigned char)R, (unsigned char)G, (unsigned char)B);
}

pixel pixel::operator/(const double& denom){
	unsigned int R = (unsigned int)(rgb[RGB_R] / denom);
	unsigned int G = (unsigned int)(rgb[RGB_G] / denom);
	unsigned int B = (unsigned int)(rgb[RGB_B] / denom);
	return pixel((unsigned char)R, (unsigned char)G, (unsigned char)B);
}

pixel pixel::operator*(const double& m){
	unsigned int R = (unsigned int)(rgb[RGB_R] * m);
	unsigned int G = (unsigned int)(rgb[RGB_G] * m);
	unsigned int B = (unsigned int)(rgb[RGB_B] * m);
	return pixel((unsigned char)R, (unsigned char)G, (unsigned char)B);
}

pixel_primitive pixel::toPixelPrimitive() {
	pixel_primitive p;
	p.rgb[RGB_B] = rgb[RGB_B];
	p.rgb[RGB_G] = rgb[RGB_G];
	p.rgb[RGB_R] = rgb[RGB_R];
	return p;
}

mask::mask(unsigned int width, unsigned int listLength, double* initList, double coefficient) {
	assert((width * width) == listLength);
	w = width;
	double* curr = initList;
	maskVals = new double*[width];
	for (unsigned int i = 0; i < width; i++) {
		maskVals[i] = new double[width];
		for (unsigned int j = 0; j < width; j++) {
			maskVals[i][j] = (*(curr++)) * coefficient;
		}
	}
}

mask::~mask() {
	for (unsigned int i = 0; i < w; i++){
		delete[] maskVals[i];
	}
	delete[] maskVals;
}

pixel pixel_primitive::toPixel() {
	return pixel(MAX(rgb[RGB_R], 0), MAX(rgb[RGB_G], 0), MAX(rgb[RGB_B], 0));
}

imageGrid::imageGrid(unsigned height, unsigned width, unsigned char* old_data) {
	pixel_primitive* old = (pixel_primitive*) old_data;
	h = height;
	w = width;
	img = new pixel*[height];
	for (unsigned int i = 0; i < h; i++) {
		img[i] = new pixel[width];
		for (unsigned int j = 0; j < w; j++) {
			img[i][j] = (old++)->toPixel();
		}
	}
}

imageGrid::imageGrid() {
	img = NULL;
	h = 0;
	w = 0;
}

imageGrid::~imageGrid(){
	if (NULL != img) {
		for (unsigned int i = 0; i < h; i++) {
			if (NULL != img[i]) {
				delete[] img[i];
			}
		}
		delete[] img;
	}
}

imageGrid& imageGrid::operator=(const imageGrid& other) {
	if (NULL != this->img){
		for (unsigned int i = 0; i < h; i++) {
			delete[] this->img[i];
			img[i] = NULL;
		}
		delete[] img;
		img = NULL;
	}
	h = other.h;
	w = other.w;
	img = new pixel*[h];
	for (unsigned int i = 0; i < h; i++) {
		img[i] = new pixel[w];
		for (unsigned int j = 0; j < w; j++) {
			img[i][j] = other.img[i][j];
		}
	}
	return *this;
}

void imageGrid::multiply(mask& _mask) {
	imageGrid buf;
	buf = *this;
	//iterate over all pixels
	for (unsigned int i = 0; i < h; i++) {
		for (unsigned int j = 0; j < w; j++) {
			buf.img[i][j] = multiplyPixel(i, j, _mask);
		}
	}
	*this = buf;
}

pixel imageGrid::multiplyPixel(unsigned int y, unsigned int x, mask& _mask) {
	pixel retVal(0, 0, 0);
	int off = _mask.w / 2;
	//iterate over all valid pixels in neighborhood
	for (int y_off = -off; y_off <= off; y_off++) {
		int y_prime = y + y_off;
		for (int x_off = -off; x_off <= off; x_off++) {
			int x_prime = x + x_off;
			if (x_prime >= 0 && x_prime < w && y_prime >= 0 && y_prime < h) {
				retVal = retVal + (img[y_prime][x_prime] * _mask.maskVals[y_off + off][x_off + off]);
			} else {
				retVal = retVal + (img[y][x] * _mask.maskVals[y_off + off][x_off + off]);
			}
		}
	}
	return retVal;
}

void imageGrid::commitImageGrid(unsigned char * old_data) {
	pixel_primitive* old = (pixel_primitive*) old_data;
	for (unsigned int i = 0; i < h; i++) {
		for (unsigned int j = 0; j < w; j++) {
			(*old++) = img[i][j].toPixelPrimitive();
		}
	}
}





