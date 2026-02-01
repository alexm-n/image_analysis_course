#include "tpConvolution.h"

cv::Mat convolution(cv::Mat image, cv::Mat kernel)
{
	cv::Mat	res;
	int		half_kernel_x;
	int		half_kernel_y;
	int		somme_kernel;
	int		img_x;
	int		img_y;
	float		new_pixel;

	res = cv::Mat::zeros(image.size(), CV_32FC1);
	half_kernel_x = kernel.rows/2;
	half_kernel_y = kernel.cols/2;
	somme_kernel = 0;
	for (int i = 0; i < kernel.rows; i++)
	{
		for (int j = 0; j < kernel.cols; j++)
		{
			somme_kernel += kernel.at<float>(i,j);
		}
	}

	for (int x = 0; x < image.rows; x++)
	{
		for (int y = 0; y < image.cols; y++)
		{
			new_pixel = 0.0;
			for (int i = 0; i < kernel.rows; i++)
			{
				for (int j = 0; j < kernel.cols; j++)
				{
					img_x = x + i - half_kernel_x;
					img_y = y + j - half_kernel_y;
					if (img_x < 0)
					{
						img_x = - img_x;
					}
					else if (img_x >= image.rows)
					{
						img_x = image.rows * 2 - img_x - 2;
					}
					if (img_y < 0)
					{
						img_y = - img_y;
					}
					else if (img_y >= image.cols)
					{
						img_y = image.cols * 2 - img_y - 2;
					}
					new_pixel += image.at<float>(img_x,img_y) * kernel.at<float>(i,j);
				}
			}
			if (somme_kernel != 0)
			{
				new_pixel /= somme_kernel;
			}
			res.at<float>(x,y) = new_pixel;
		}
	}
	return res;
}
