#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <algorithm>

int main(int ac, char** av)
{
	if (ac != 2)
	{
		std::cerr << "Error: usage." << std::endl;
		return -1;
	}

	cv::Mat input_img = cv::imread(av[1], cv::IMREAD_GRAYSCALE);
	if (input_img.empty())
	{
		std::cerr << "Error: empty image." << std::endl;
		return -1;
	}

	input_img.convertTo(input_img, CV_32FC1, 1.0 / 255.0);

	//Caractéristiques du filtre médian
	int ksize = 3;
	int half = ksize / 2;

	cv::Mat output = cv::Mat::zeros(input_img.size(), CV_32FC1);

	for (int x = 0; x < input_img.rows; x++)
	{
		for (int y = 0; y < input_img.cols; y++)
		{
			std::vector<float> my_tab;
			my_tab.reserve(ksize * ksize);

			for (int i = -half; i <= half; i++)
			{
				for (int j = -half; j <= half; j++)
				{
					int img_x = x + i;
					int img_y = y + j;
					if (img_x < 0) img_x = -img_x;
					else if (img_x >= input_img.rows)
						img_x = input_img.rows * 2 - img_x - 2;
					if (img_y < 0) img_y = -img_y;
					else if (img_y >= input_img.cols)
						img_y = input_img.cols * 2 - img_y - 2;
					my_tab.push_back(input_img.at<float>(img_x, img_y));
				}
			}
			std::sort(my_tab.begin(), my_tab.end());
			output.at<float>(x,y) = my_tab[my_tab.size() / 2];
		}
	}

	cv::imshow("Input", input_img);
	cv::imshow("Median filter", output);

	cv::Mat output_8u;
	output.convertTo(output_8u, CV_8UC1, 255.0); 
	cv::imwrite("data/out.png", output_8u);

	cv::waitKey(0);
	return 0;
}
