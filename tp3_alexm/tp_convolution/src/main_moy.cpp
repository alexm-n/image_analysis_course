#include "tpConvolution.h"

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

	cv::Mat kernel = cv::Mat::ones(11, 11, CV_32FC1);

	cv::imshow("input image", input_img);
	cv::Mat output = convolution(input_img, kernel);
	cv::imshow("output image", output);

	cv::Mat output_8u;
	output.convertTo(output_8u, CV_8UC1, 255.0); 
	cv::imwrite("data/out.png", output_8u);

	cv::waitKey(0);

	return 0;
}
