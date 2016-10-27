#include <assert.h>
#include <math.h>
#include <iostream>

#include "pixel.h"

#define MAX_CHAR (~((unsigned char) 0))

#define MAX(a,b) ((a > b)? a : b)
#define MIN(a,b) ((a < b)? a : b)
#define RANGE(LOW, X, HIGH) MAX(MIN(X, HIGH), 0)
#define MAX_COLOR (255.0)
#define CAP(X) RANGE(0, X, MAX_COLOR)
#define NUM_COLORS (3.0)
#define PI (3.141592653589793238462643383279502)
#define tPI (2 * PI)
#define EPS (2.71828182846)

#define ASSERT(M)	{																			\
	if (false == (M)) {																			\
		std::cout << "Error at: " << #M << " at " << __LINE__ << ":" << __FILE__ << std::endl;	\
		exit(0);																				\
	}																							\
}																								\

enum BGR {
	RGB_B = 0,
	RGB_G = 1,
	RGB_R = 2
};

enum HSI {
	HSI_H = RGB_R,
	HSI_S = RGB_G,
	HSI_I = RGB_B
};

pixel::pixel(int R, int G, int B) {
	rgb[RGB_R] = R;
	rgb[RGB_G] = G;
	rgb[RGB_B] = B;
}

pixel::pixel() {
	rgb[RGB_R] = 0;
	rgb[RGB_G] = 0;
	rgb[RGB_B] = 0;
}

pixelPrecision pixel::getI() {
	pixelPrecision ret = 0;
	for (int i = 0; i < PIX_ARR_SIZE; i++) {
		ret += (rgb[i] / PIX_ARR_SIZE);
	}
	return ret;
}

void pixel::setI(pixelPrecision I) {
	for (int i = 0; i < PIX_ARR_SIZE; i++) {
		rgb[i] = I;
	}
}

pixelPrecision inBounds(pixelPrecision p) {
	while (p < 0)    { p += tPI; }
	while (p >= tPI) { p -= tPI; }
	if (p < 0) { p += tPI; }
	if (isnan(p)) { p = 0; }
	ASSERT(p >= 0);
	ASSERT(p < tPI);
	return p;
}

pixel pixel::RGB_toI() {
	pixel ret = this->RGB_toHSI();
	ret.rgb[HSI_H] = ret.rgb[HSI_I];
	ret.rgb[HSI_S] = ret.rgb[HSI_I];
	return ret;
}

pixel pixel::RGB_toHSI() {
	pixel ret(0, 0, 0);
	pixelPrecision B = rgb[RGB_B];
    pixelPrecision G = rgb[RGB_G];
    pixelPrecision R = rgb[RGB_R];
    pixelPrecision H, S, I, a;

    a = MIN(R, MIN(G, B));
    I = ( R + G + B) / 3.0;
    S = 1 - 3.0/(R+G+B)*a;
    if (S == 0.0) {
        H = 0.0;
    } else {
        if (B <= G){
            H = acos((((R-G)+(R-B))/2.0)/(sqrt((R-G)*(R-G) + (R-B)*(G-B))));
        } else {
            if (B > G)
            H = 2*PI - acos((((R-G)+(R-B))/2.0)/(sqrt((R-G)*(R-G) + (R-B)*(G-B))));
        }
    }

    ret.rgb[HSI_H] = H;
    ret.rgb[HSI_S] = S;
    ret.rgb[HSI_I] = I;
	return ret;
}

pixel pixel::HSI_toRGB() {
	pixel ret(0,0,0);
	pixelPrecision H = inBounds(rgb[HSI_H]);
	pixelPrecision S = rgb[HSI_S];
	pixelPrecision I = rgb[HSI_I];
	pixelPrecision R, G, B;
	pixelPrecision tmp;
	if (H == 0) {
		R = (I + 2 * S * I);
		G = (I - I * S);
		B = (I - I * S);
	} else if (H < tPI / 3) {
		tmp = cos(H) / cos(PI / 3 - H);
		R = I + I * S * tmp;
		G = I + I * S * (1 - tmp);
		B = I - I * S;
	} else if (H == tPI / 3) {
		R = I - I * S;
		G = I + 2 * I * S;
		B = I - I * S;
	} else if (H < 2 * tPI / 3) {
		tmp = cos(H - tPI / 3) / cos(PI - H);
		R = I - I * S;
		G = I + I * S * tmp;
		B = I + I * S * (1 - tmp);
	} else if (H == 2 * tPI / 3) {
		R = I - I * S;
		G = I - I * S;
		B = I + 2 * I * S;
	} else if (H > 2 * tPI / 3) {
		tmp = cos(H - 4 * PI / 3) / cos(5 * PI / 3 - H);
		R = I + I * S * (1 - tmp);
		G = I - I * S;
		B = I + I * S * tmp;
	} else {
		//std::cout << "ERROR!\n";
		//exit(0);
	}
	ret.rgb[RGB_R] = R;
	ret.rgb[RGB_G] = G;
	ret.rgb[RGB_B] = B;
	return ret;
}

pixel pixel::operator+(const pixel& neighbor){
	pixelPrecision R = rgb[RGB_R] + neighbor.rgb[RGB_R];
	pixelPrecision G = rgb[RGB_G] + neighbor.rgb[RGB_G];
	pixelPrecision B = rgb[RGB_B] + neighbor.rgb[RGB_B];
	return pixel(R, G, B);
}

pixel pixel::operator-(const pixel& neighbor){
	pixelPrecision R = rgb[RGB_R] - neighbor.rgb[RGB_R];
	pixelPrecision G = rgb[RGB_G] - neighbor.rgb[RGB_G];
	pixelPrecision B = rgb[RGB_B] - neighbor.rgb[RGB_B];
	return pixel(R, G, B);
}

pixel pixel::operator/(const double& denom){
	assert(denom != 0.0);
	return (*this) * (1.0 / denom);
}

pixel pixel::operator*(const double& m){
	pixelPrecision R = rgb[RGB_R] * m;
	pixelPrecision G = rgb[RGB_G] * m;
	pixelPrecision B = rgb[RGB_B] * m;
	return pixel(R, G, B);
}

pixel pixel::operator*(const pixel& m) {
	pixelPrecision R = rgb[RGB_R] * rgb[RGB_R];
	pixelPrecision G = rgb[RGB_G] * rgb[RGB_G];
	pixelPrecision B = rgb[RGB_B] * rgb[RGB_B];
	return pixel(R, G, B);
}

pixel pixel::root() {
	pixelPrecision R = sqrt(rgb[RGB_R]);
	pixelPrecision G = sqrt(rgb[RGB_G]);
	pixelPrecision B = sqrt(rgb[RGB_B]);
	return pixel(R, G, B);
}

pixel_primitive pixel::toPixelPrimitive() {
	pixel_primitive p;
	p.rgb[RGB_B] = CAP(rgb[RGB_B]);
	p.rgb[RGB_G] = CAP(rgb[RGB_G]);
	p.rgb[RGB_R] = CAP(rgb[RGB_R]);
	return p;
}

mask::mask() {
	w = 0;
	maskVals = NULL;
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

mask& mask::operator=(const mask& other) {
	if (NULL != this->maskVals){
		for (unsigned int i = 0; i < w; i++) {
			delete[] this->maskVals[i];
			maskVals[i] = NULL;
		}
		delete[] maskVals;
		maskVals = NULL;
	}
	w = other.w;
	maskVals = new double*[w];
	for (unsigned int i = 0; i < w; i++) {
		maskVals[i] = new double[w];
		for (unsigned int j = 0; j < w; j++) {
			maskVals[i][j] = other.maskVals[i][j];
		}
	}
	return *this;
}

double mask::LOG(double x, double y, double sigma) {
	double sig2 = sigma * sigma;
	double sig4 = sig2 * sig2;
	double x2 = x * x;
	double y2 = y * y;
    double term1 = (-1.0 / (PI * sig4));
    double f_xy = ((x2 + y2) / (2.0 * sig2));
    double term2 = 1 - f_xy;
    double term3 = pow(EPS, -f_xy);
    return (term1 * term2 * term3);
}

mask mask::makeLOG(int width, double sigma) {
    //I don't wanna define this for even block size
    assert(width % 2 == 1);
    
    // Create the data array
    double* list = new double[width * width];

    double scaleFactor = MAX_COLOR;//-(MAX_COLOR) / LOG(0, 0, sigma);

    // Work out the "min and max" values. Log is centered around 0, 0
    // so, for a size 5 template (say) we want to get the values from
    // -2 to +2, ie: -2, -1, 0, +1, +2 and feed those into the formula.
    int min = -(width / 2);
    int max = (width / 2);

    // We also need a count to index into the data array...
    int xCount = 0;
    int yCount = 0;
    int lCount = 0;

    for(int x = min; x <= max; ++x) {
        for(int y = min; y <= max; ++y) {
            // Get the LoG value for this (x,y) pair
        	double ww = 1.0;
            double lx = (x / ww);
            double ly = (y / ww);
            list[lCount++] = LOG(lx, ly, sigma) * scaleFactor;
            ++yCount;
        }
        ++xCount;
    }

    mask retVal(width, width * width, list);
    delete[] list;
    return retVal;
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

void imageGrid::RGB_toI() {
	for (unsigned int i = 0; i < h; i++){
		for (unsigned int j = 0; j < w; j++) {
			img[i][j] = img[i][j].RGB_toI();
		}
	}
}

unsigned round_up(unsigned val, unsigned unit) {
	return ((val + unit - 1) / unit) * unit;
}

eight_block ComputeIDct(const pixelPrecision in[block_size][block_size]/*, double& out[block_size][block_size]*/) {
	pixelPrecision out[block_size][block_size];

	for (unsigned i = 0; i < block_size; i++) {
		for (unsigned j = 0; j < block_size; j++) {
			pixelPrecision& s = out[i][j];
			s = 0;

			for (unsigned u = 0; u < block_size; u++) {
				for (unsigned v = 0; v < block_size; v++) {
          			s += in[u][v] * cos((2 * i + 1) * u * M_PI / 16) * cos((2 * j + 1) * v * M_PI / 16) *
          				((u == 0) ? 1 / sqrt(2) : 1.) * ((v == 0) ? 1 / sqrt(2) : 1.);
				}
			}

      		s /= 4;
      	}
    }
    eight_block ret = *((eight_block*)&out);
    return ret;
}

eight_block ComputeDct(const pixelPrecision in[block_size][block_size]/*, double& out[block_size][block_size]*/) {
	pixelPrecision out[block_size][block_size];

	for (unsigned i = 0; i < block_size; i++) {
		for (unsigned j = 0; j < block_size; j++) {
			pixelPrecision& s = out[i][j];
			s = 0;

			for (unsigned u = 0; u < block_size; u++) {
				for (unsigned v = 0; v < block_size; v++) {
					s += in[u][v] * cos((2 * u + 1) * i * M_PI / 16) * cos((2 * v + 1) *
						j * M_PI / 16) * ((i == 0) ? 1 / sqrt(2) : 1) * ((j == 0) ? 1 / sqrt(2) : 1);
				}
			}

      		s /= 4;
      	}
    }
    eight_block ret = *((eight_block*)&out);
    return ret;
}

void imageGrid::DCT() {
	unsigned h_prime = round_up(h, block_size);
	const unsigned h_blocks = h_prime / block_size;
	unsigned w_prime = round_up(w, block_size);
	const unsigned w_blocks = w_prime / block_size;

	eight_block block_grid[h_blocks][w_blocks];

	//setup all block_grids
	for (unsigned i = 0; i < h_blocks; i++) {
		for (unsigned j = 0; j < w_blocks; j++) {
			for (unsigned y = 0; y < block_size; y++) {
				for (unsigned x = 0; x < block_size; x++) {
					block_grid[i][j].block_arr[y][x] = img[i * block_size + y][j * block_size + x].getI();
				}
			}
		}
	}

	//process each block_grid
	for (unsigned i = 0; i < h_blocks; i++) {
		for (unsigned j = 0; j < w_blocks; j++) {
			block_grid[i][j] = ComputeDct(block_grid[i][j].block_arr);
			if (i % 2 != j % 2) {
				//block_grid[i][j] = ComputeIDct(block_grid[i][j].block_arr);
			}
		}
	}

	//move all block_grids back into imageGrid
	for (unsigned i = 0; i < h_blocks; i++) {
		for (unsigned j = 0; j < w_blocks; j++) {
			for (unsigned y = 0; y < block_size; y++) {
				for (unsigned x = 0; x < block_size; x++) {
					if ( i * block_size + y < h && j * block_size + x < w) {
						img[i * block_size + y][j * block_size + x].setI(block_grid[i][j].block_arr[y][x]);
					}
				}
			}
		}
	}
}

void imageGrid::RGB_toHSI() {
	for (unsigned int i = 0; i < h; i++){
		for (unsigned int j = 0; j < w; j++) {
			img[i][j] = img[i][j].RGB_toHSI();
		}
	}
}

void imageGrid::HSI_toRGB() {
	for (unsigned int i = 0; i < h; i++){
		for (unsigned int j = 0; j < w; j++) {
			img[i][j] = img[i][j].HSI_toRGB();
		}
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

void imageGrid::sobel() {

    double sob_x[] = { -1,  0,  1,
                       -2,  0,  2,
                       -1,  0,  1  };

    double sob_y[] = { -1, -2, -1,
                        0,  0,  0,
                        1,  2,  1  };

	mask sobelX(3, sizeof(sob_x) / sizeof(double), sob_x, 1 / 1.0);
    mask sobelY(3, sizeof(sob_y) / sizeof(double), sob_y, 1 / 1.0);

    imageGrid gx;
    imageGrid gy;
    gx = *this;
    gy = *this;

    gx.multiply(sobelX);
    gy.multiply(sobelY);

    for (unsigned int i = 0; i < h; i++) {
		for (unsigned int j = 0; j < w; j++) {
			img[i][j] = (gx.img[i][j] * gx.img[i][j] + gy.img[i][j] * gy.img[i][j]).root();
		}
	}
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





