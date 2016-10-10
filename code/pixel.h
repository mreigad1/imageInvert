class pixel;
class pixel_primitive;
class image;
class mask;

//pixel stores high precision pixel data during
//mathematical operations to avoid overflows
class pixel {
	public:
		pixel();
		pixel(unsigned char R, unsigned char G, unsigned char B );
		pixel operator+(const pixel& neighbor);
		pixel operator-(const pixel& neighbor);
		pixel operator/(const double& denom);
		pixel operator*(const double& m);
	private:
		static const int PIX_ARR_SIZE = 3;
		unsigned int rgb[PIX_ARR_SIZE];
};

//pixel primitive is a type with size and alignment matching native pixel
//implementation and provides a mechanism for casting to higher precision type
class pixel_primitive{
	public:
		pixel toPixel();
	private:
		static const int pp_size = 3;
		unsigned char rgb[pp_size];
};

//image stores a copy of an image to be operated upon and allows copying
//back to image buffer following operations
class image {
	public:
		image(unsigned height, unsigned width, pixel_primitive* old);
		~image();
		image& operator=(const image& other);
		void multiply(mask& _mask);
		pixel multiplyPixel(unsigned int y, unsigned int x, mask& _mask);
	private:
		unsigned int h;
		unsigned int w;
		pixel** img;
};

//mask stores data for square masks filters
class mask {
	public:
		mask(unsigned int width, unsigned int listLength, double* initList);
		friend pixel image::multiplyPixel(unsigned int y, unsigned int x, mask& _mask);
		~mask();
	private:
		unsigned int w;
		double** maskVals;
};

