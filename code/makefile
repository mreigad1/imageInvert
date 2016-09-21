VERSION=2.4.13_3
LIB_PATHS=-L/usr/local/Cellar/opencv/${VERSION}/lib
LIBS=-lopencv_highgui -lopencv_core -lopencv_imgproc
all: histogram

histogram: histogram.cpp
	g++ histogram.cpp ${LIB_PATHS} ${LIBS} -o histogram

.PHONY:clean

clean:
	rm histogram
