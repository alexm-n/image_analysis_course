#include "tpMorphology.h"
#include <cmath>
#include <algorithm>
#include <tuple>
#include <limits>
#include "common.h"
using namespace cv;
using namespace std;


/**
    Compute a median filter of the input float image.
    The filter window is a square of (2*size+1)*(2*size+1) pixels.

    Values outside image domain are ignored.

    The median of a list l of n>2 elements is defined as:
     - l[n/2] if n is odd 
     - (l[n/2-1]+l[n/2])/2 is n is even 
*/
Mat median(Mat image, int size)
{
    Mat res = image.clone();
    assert(size>0);
    /********************************************
                YOUR CODE HERE
    *********************************************/
    
    /********************************************
                END OF YOUR CODE
    *********************************************/
    return res;
}


/**
    Compute the dilation of the input float image by the given structuring element.
     Pixel outside the image are supposed to have value 0
*/
Mat dilate(Mat image, Mat structuringElement)
{
    Mat res = Mat::zeros(image.rows,image.cols,CV_32FC1);
    /********************************************
                YOUR CODE HERE
    *********************************************/

    float v_max;
    float v_img;
    int center_y = structuringElement.rows / 2;
    int center_x = structuringElement.cols / 2;

    for (int y = 0; y < image.rows; y++)
    {
        for (int x = 0; x < image.cols; x++)
        {
            v_max = 0.0;
            for (int j = 0; j < structuringElement.rows; j++)
            {
                for (int i = 0; i < structuringElement.cols; i++)
                {
                    if (structuringElement.at<float>(j,i) != 0.0)
                    {
                        int tmp_y = y + j - center_y;
                        int tmp_x = x + i - center_x;

                        if (tmp_y >= 0 && tmp_y < image.rows && tmp_x >= 0 && tmp_x < image.cols)
                        {
                            v_img = image.at<float>(tmp_y, tmp_x);
                        }
                        else
                        {
                            v_img = 0.0;
                        }
                        if (v_max < v_img)
                        {
                            v_max = v_img;
                        }
                    }
                }
            }
            res.at<float>(y,x) = v_max;
        }
    }
     
    /********************************************
                END OF YOUR CODE
    *********************************************/
    return res;
}


/**
    Compute the erosion of the input float image by the given structuring element.
    Pixel outside the image are supposed to have value 1.
*/
Mat erode(Mat image, Mat structuringElement)
{
    Mat res = image.clone();
    /********************************************
                YOUR CODE HERE
        hint : 1 line of code is enough
    *********************************************/

    res = 1.0 - dilate(1.0 - image, structuringElement);
    
    /********************************************
                END OF YOUR CODE
    *********************************************/
    return res;
}


/**
    Compute the opening of the input float image by the given structuring element.
*/
Mat open(Mat image, Mat structuringElement)
{

    Mat res = Mat::zeros(image.rows, image.cols, CV_32FC1);
    /********************************************
                YOUR CODE HERE
        hint : 1 line of code is enough
    *********************************************/
    
    res = dilate(erode(image, structuringElement), structuringElement);

    /********************************************
                END OF YOUR CODE
    *********************************************/
    return res;
}


/**
    Compute the closing of the input float image by the given structuring element.
*/
Mat close(Mat image, Mat structuringElement)
{

    Mat res = Mat::zeros(image.rows, image.cols, CV_32FC1);
    /********************************************
                YOUR CODE HERE
        hint : 1 line of code is enough
    *********************************************/

    res = erode(dilate(image, structuringElement), structuringElement);
    
    /********************************************
                END OF YOUR CODE
    *********************************************/
    return res;
}


/**
    Compute the morphological gradient of the input float image by the given structuring element.
*/
Mat morphologicalGradient(Mat image, Mat structuringElement)
{

    Mat res = Mat::zeros(image.rows, image.cols, CV_32FC1);
    /********************************************
                YOUR CODE HERE
        hint : 1 line of code is enough
    *********************************************/

    res = dilate(image, structuringElement) - erode(image, structuringElement);
    
    /********************************************
                END OF YOUR CODE
    *********************************************/
    return res;
}

