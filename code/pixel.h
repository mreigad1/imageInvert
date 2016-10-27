class pixel;
class pixel_primitive;
class imageGrid;
class mask;

typedef double pixelPrecision;

//pixel stores high precision pixel data during
//mathematical operations to avoid overflows
class pixel {
	public:
		pixel();
		pixel(int R, int G, int B);
		pixel operator+(const pixel& neighbor);
		pixel operator-(const pixel& neighbor);
		pixel operator/(const double& denom);
		pixel operator*(const double& m);
		pixel operator*(const pixel& m);
		pixel root();
		pixel_primitive toPixelPrimitive();
		pixel RGB_toI();
		pixel RGB_toHSI();
		pixel HSI_toRGB();
		pixelPrecision getI();
		void setI(pixelPrecision I);
	private:
		static const int PIX_ARR_SIZE = 3;
		pixelPrecision rgb[PIX_ARR_SIZE];
};

//pixel primitive is a type with size and alignment matching native pixel
//implementation and provides a mechanism for casting to higher precision type
class pixel_primitive {
	public:
		pixel toPixel();
		friend pixel_primitive pixel::toPixelPrimitive();
	private:
		static const int pp_size = 3;
		unsigned char rgb[pp_size];
};

//imageGrid stores a copy of an imageGrid to be operated upon and allows copying
//back to imageGrid buffer following operations
class imageGrid {
	public:
		imageGrid();
		imageGrid(unsigned height, unsigned width, unsigned char* old_data);
		~imageGrid();
		imageGrid& operator=(const imageGrid& other);
		void multiply(mask& _mask);
		void sobel();
		pixel multiplyPixel(unsigned int y, unsigned int x, mask& _mask);
		void DCT();
		void RGB_toI();
		void RGB_toHSI();
		void HSI_toRGB();
		void commitImageGrid(unsigned char* old_data);
	private:
		unsigned int h;
		unsigned int w;
		pixel** img;
};

//mask stores data for square masks filters
class mask {
	public:
		mask();
		mask(unsigned int width, unsigned int listLength, double* initList, double coefficient = 1.0);
		friend pixel imageGrid::multiplyPixel(unsigned int y, unsigned int x, mask& _mask);
		static double LOG(double x, double y, double sigma);
		static mask makeLOG(int width, double sigma);
		~mask();
		mask& operator=(const mask& other);
	private:
		unsigned int w;
		double** maskVals;
};

const unsigned block_size = 8;
struct eight_block {
	double block_arr[block_size][block_size];
};