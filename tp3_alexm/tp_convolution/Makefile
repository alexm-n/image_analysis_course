CC = g++
CFLAGS = -std=c++17 -Werror -Wextra -Wall -Wpedantic
OPENCV = `pkg-config --cflags --libs opencv4`
SRC_MOY = src/tpConvolution.cpp src/main_moy.cpp
SRC_SOB = src/tpConvolution.cpp src/main_sobel.cpp
SRC_MEDIAN = src/main_median.cpp


all: moy sob median
	
moy: $(SRC_MOY)
	$(CC) $(CFLAGS) $(SRC_MOY) -o bin/moyenneur $(OPENCV)

sob: $(SRC_SOB)
	$(CC) $(CFLAGS) $(SRC_SOB) -o bin/grad_sobel $(OPENCV)

median: $(SRC_MEDIAN)
	$(CC) $(CFLAGS) $(SRC_MEDIAN) -o bin/median $(OPENCV)

clean:
	rm -f bin/moyenneur
	rm -f bin/grad_sobel
	rm -f bin/median
	rm -f data/out.png

re: clean all

.PHONY: all clean re moy sob median
