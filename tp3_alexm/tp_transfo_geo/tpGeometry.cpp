#include "tpGeometry.h"
#include <cmath>
#include <algorithm>
#include <tuple>
using namespace cv;
using namespace std;

/**
    Transpose the input image,
    ie. performs a planar symmetry according to the
    first diagonal (upper left to lower right corner).
*/
Mat transpose(Mat image)
{
    Mat res = Mat::zeros(image.cols,image.rows,CV_32FC1);
    /********************************************
                YOUR CODE HERE
    hint: consider a non square image
    *********************************************/

    for (int i = 0; i < image.rows; i++)
    {
        for (int j = 0; j < image.cols; j++)
        {
            res.at<float>(j, i) = image.at<float>(i, j);
        }
    }
   
    /********************************************
                END OF YOUR CODE
    *********************************************/
    return res;
}

/**
    Compute the value of a nearest neighbour interpolation
    in image Mat at position (x,y)
*/
float interpolate_nearest(Mat image, float y, float x)
{
    float v=0;
    /********************************************
                YOUR CODE HERE
    *********************************************/
    
    int xint = (int)(x + 0.5);
    int yint = (int)(y + 0.5);

    if (xint < 0)
    {
        xint = 0;
    }
    if (xint >= image.cols)
    {
        xint = image.cols - 1;
    }
    if (yint < 0)
    {
        yint = 0;
    }
    if (yint >= image.rows)
    {
        yint = image.rows - 1;
    }

    v = image.at<float>(yint, xint);

    /********************************************
                END OF YOUR CODE
    *********************************************/
    return v;

}


/**
    Compute the value of a bilinear interpolation in image Mat at position (x,y)
*/
float interpolate_bilinear(Mat image, float y, float x)
{
    float v=0;
    /********************************************
                YOUR CODE HERE
    *********************************************/

    int x1 = (int)x;
    int y1 = (int)y;
    int x2 = x1 + 1;
    int y2 = y1 + 1;

    if (x1 < 0)
    {
        x1 = 0;
    }
    if (x2 >= image.cols)
    {
        x2 = image.cols - 1;
    }
    if (y1 < 0)
    {
        y1 = 0;
    }
    if (y2 >= image.rows)
    {
        y2 = image.rows - 1;
    }

    float alpha = x - x1;
    float beta  = y - y1;

    float f11 = image.at<float>(y1, x1);
    float f21 = image.at<float>(y1, x2);
    float f12 = image.at<float>(y2, x1);
    float f22 = image.at<float>(y2, x2);

    v = (1 - alpha) * (1 - beta) * f11 + alpha * (1 - beta) * f21 + (1 - alpha) * beta * f12 + alpha * beta * f22;
    
    /********************************************
                END OF YOUR CODE
    *********************************************/
    return v;
}

/**
    Multiply the image resolution by a given factor using the given interpolation method.
    If the input size is (h,w) the output size shall be ((h-1)*factor, (w-1)*factor)
*/
Mat expand(Mat image, int factor, float(* interpolationFunction)(cv::Mat image, float y, float x))
{
    assert(factor>0);
    Mat res = Mat::zeros((image.rows-1)*factor,(image.cols-1)*factor,CV_32FC1);
    /********************************************
                YOUR CODE HERE
    *********************************************/
    
    for (int i = 0; i < res.rows; i++)
    {
        for (int j = 0; j < res.cols; j++)
        {
            float y_src = i / (float)factor;
            float x_src = j / (float)factor;
            res.at<float>(i,j) = interpolationFunction(image, y_src, x_src);
        }
    }
    
    /********************************************
                END OF YOUR CODE
    *********************************************/
    return res;
}

/**
    Performs a rotation of the input image with the given angle (clockwise) and the given interpolation method.
    The center of rotation is the center of the image.

    Ouput size depends of the input image size and the rotation angle.

    Output pixels that map outside the input image are set to 0.
*/
Mat rotate(Mat image, float angle, float(* interpolationFunction)(cv::Mat image, float y, float x))
{
    /********************************************
                YOUR CODE HERE
    hint: to determine the size of the output, take
    the bounding box of the rotated corners of the 
    input image.
    *********************************************/

    int h = image.rows;
    int w = image.cols;
    float cx = (h - 1) / 2.0;
    float cy = (w - 1) / 2.0;

    angle = -angle * 3.14159265358979323846f / 180.0;

    float sinA = std::sin(angle);
    float cosA = std::cos(angle);

    float corners_x[4] = {-cx, h-1-cx, h-1-cx, -cx};
    float corners_y[4] = {-cy, -cy, w-1-cy, w-1-cy};

    float x_rot[4], y_rot[4];
    float x_min = 1e10f, x_max = -1e10f;
    float y_min = 1e10f, y_max = -1e10f;

    for (int i = 0; i < 4; i++)
    {
        x_rot[i] = corners_x[i] * cosA - corners_y[i] * sinA;
        y_rot[i] = corners_x[i] * sinA + corners_y[i] * cosA;

        if (x_rot[i] < x_min)
        {
            x_min = x_rot[i];
        }
        if (x_rot[i] > x_max)
        {
            x_max = x_rot[i];
        }
        if (y_rot[i] < y_min)
        {
            y_min = y_rot[i];
        }
        if (y_rot[i] > y_max)
        {
            y_max = y_rot[i];
        }
    }

    int new_h = (int)(x_max - x_min + 0.5);
    int new_w = (int)(y_max - y_min + 0.5);
    
    Mat res = Mat::zeros(new_h, new_w, CV_32FC1);

    float cx_new = (new_h - 1) / 2.0;
    float cy_new = (new_w - 1) / 2.0;

    for (int x_out = 0; x_out < new_h; x_out++)
    {
        for (int y_out = 0; y_out < new_w; y_out++)
        {
            float x_c = x_out - cx_new;
            float y_c = y_out - cy_new;

            float x_orig = x_c * cosA + y_c * sinA + cx;
            float y_orig = -x_c * sinA + y_c * cosA + cy;

            if (x_orig >= 0 && x_orig < h && y_orig >= 0 && y_orig < w)
            {
                res.at<float>(x_out, y_out) = interpolationFunction(image, x_orig, y_orig);
            }
        }
    }
    
    /********************************************
                END OF YOUR CODE
    *********************************************/
    return res;

}