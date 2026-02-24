CC = g++
CFLAGS = -std=c++17 -Werror -Wextra -Wall -Wpedantic
OPENCV = `pkg-config --cflags --libs opencv4`


all: main

main: main.cpp
	$(CC) $(CFLAGS) main.cpp -o coin_detector $(OPENCV)

clean:
	rm -f coin_detector

re: clean all

.PHONY: all clean re main