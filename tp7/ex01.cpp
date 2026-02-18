#include <opencv2/opencv.hpp>
#include <cmath>
#include <iostream>

cv::Mat	setGrayScale(cv::Mat input, int nb_grayscale_values) {
	cv::Mat output = cv::Mat::zeros(input.rows, input.cols, CV_8U);
	for (int y = 0; y < input.rows; y++) {
		for (int x = 0; x < input.cols; x++) {
			unsigned char pixel = input.at<unsigned char>(y,x);
			int new_pixel = (pixel * nb_grayscale_values) / 256;
			if (new_pixel >= nb_grayscale_values) {
				new_pixel = nb_grayscale_values - 1;
			}
			output.at<unsigned char>(y,x) = (unsigned char)new_pixel;
		}
	}
	return output;
}

cv::Mat	makeCGLM(const cv::Mat& window, int nb_grayscale_values) {
	cv::Mat cglm = cv::Mat::zeros(nb_grayscale_values, nb_grayscale_values, CV_32F);
	for (int y = 0; y < window.rows; y++) {
		for (int x = 0; x < window.cols - 1; x++) {
			int i = window.at<uchar>(y,x);
			int j = window.at<uchar>(y,x+1);
			cglm.at<float>(i,j) += 1.0;
		}
	}
	float sum = 0.0;
	for (int i = 0; i < nb_grayscale_values; i++) {
		for (int j = 0; j < nb_grayscale_values; j++) {
			sum += cglm.at<float>(i,j);
		}
	}
	if (sum > 0) {
		cglm /= sum;
	}
	return cglm;
}

float	getVariance(const cv::Mat& cglm, int nb_grayscale_values) {
	float mean = 0.0;
	for (int i = 0; i < nb_grayscale_values; i++) {
		for (int j = 0; j < nb_grayscale_values; j++) {
			mean += i * cglm.at<float>(i,j);
		}
	}
	float variance = 0.0;
	for (int i = 0; i < nb_grayscale_values; i++) {
		for (int j = 0; j < nb_grayscale_values; j++) {
			variance += (i - mean) * (i - mean) * cglm.at<float>(i,j);
		}
	}
	return variance;
}

float	getContrast(const cv::Mat& cglm, int nb_grayscale_values) {
	float contrast = 0.0;
	for (int i = 0; i < nb_grayscale_values; i++) {
		for (int j = 0; j < nb_grayscale_values; j++) {
			contrast += (i - j) * (i - j) * cglm.at<float>(i,j);
		}
	}
	return contrast;
}

float	getEntropy(const cv::Mat& cglm, int nb_grayscale_values) {
	float entropy = 0.0;
	for (int i = 0; i < nb_grayscale_values; i++) {
		for (int j = 0; j < nb_grayscale_values; j++) {
			float p = cglm.at<float>(i,j);
			if (p > 0) {
				entropy += p * log2(p);
			}
		}
	}
	return entropy * -1;
}

int		main(int ac, char** av) {
	if (ac != 2) {
		std::cerr << "Error usage : ./exe <image_path>" << std::endl;
		return -1;
	}

	cv::Mat input = cv::imread(av[1], cv::IMREAD_GRAYSCALE);

	if (input.empty()) {
		std::cerr << "Error : Empty image." << std::endl;
		return -1;
	}

	int nb_grayscale_values = 8;
//	int	windowSize = input.rows;
//	int halfWS = windowSize / 2;

	cv::Mat quantized = setGrayScale(input, nb_grayscale_values);
//	for (int y = halfWS; y < input.rows - halfWS; y++) {
//		for (int x = halfWS; x < input.cols - halfWS; x++) {
//			cv::Rect roi(x - halfWS, y - halfWS, windowSize, windowSize);
//			cv::Mat window = quantized(roi);
	cv::Mat cglm = makeCGLM(quantized, nb_grayscale_values);
//		}
//	}

	float variance = getVariance(cglm, nb_grayscale_values);
	float contrast = getContrast(cglm, nb_grayscale_values);
	float entropy = getEntropy(cglm, nb_grayscale_values);

	std::cout << "variance globale : " << variance << ", contraste global : " << contrast << " et entropie globale : " << entropy << std::endl;

	//cv::Mat display;
	//output.convertTo(display, CV_8U, 255.0 / (nb_grayscale_values - 1));
	//cv::imshow("Output", display);
	//cv::waitKey(0);

	return 0;
}