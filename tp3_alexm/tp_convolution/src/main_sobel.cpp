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

	cv::Mat sobel_x = (cv::Mat_<float>(3,3) <<
		-1, 0, 1,
		-2, 0, 2,
		-1, 0, 1
	);
	cv::Mat sobel_y = (cv::Mat_<float>(3,3) <<
		-1, -2, -1,
		 0,  0,  0,
		 1,  2,  1
	);

	cv::Mat grad_x = convolution(input_img, sobel_x);
	cv::Mat grad_y = convolution(input_img, sobel_y);
	cv::Mat norme_sobel(input_img.size(), CV_32FC1);
	for (int i = 0; i < input_img.rows; i++)
	{
		for (int j = 0; j < input_img.cols; j++)
		{
			float gx = grad_x.at<float>(i,j);
			float gy = grad_y.at<float>(i,j);
			norme_sobel.at<float>(i,j) = std::sqrt(gx * gx + gy * gy);
		}
	}

	cv::imshow("Input", input_img);
	cv::imshow("Output", norme_sobel);

	cv::Mat output_8u;
	norme_sobel.convertTo(output_8u, CV_8UC1, 255.0); 
	cv::imwrite("data/out.png", output_8u);

	cv::waitKey(0);
	return 0;
}