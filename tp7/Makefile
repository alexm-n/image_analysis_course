CC = g++
CFLAGS = -std=c++17 -Werror -Wextra -Wall -Wpedantic
OPENCV = `pkg-config --cflags --libs opencv4`


all: ex01

ex01: ex01.cpp
	$(CC) $(CFLAGS) ex01.cpp -o ex01 $(OPENCV)

clean:
	rm -f exO1

re: clean all

.PHONY: all clean re ex01