#include<opencv2/opencv.hpp>
#include<iostream>
#include<string>
#include<vector>
#include<queue>
#include<cmath>
#include<iomanip>

using namespace std;
using namespace cv;
double deg(double x)
{
    return atan(x) * 180.0 / CV_PI;
}

void PreProcess(Mat& image, Mat& result);
double cal_len(Point2f p[], double slope[], Mat image);

int main(int argc, char* argv[])
{
    String image_path = argv[1];
    Mat img = imread(image_path);
    Mat result = img.clone();
    cvtColor(img, img, COLOR_BGR2GRAY);
    PreProcess(img, result);
}

void PreProcess(Mat& image, Mat& result)
{
    threshold(image, image, 128, 255, THRESH_BINARY);
    GaussianBlur(image, image, Size(3, 3), 0, 1);
    Canny(image, image, 20, 10);

    vector<Point> con[2];  // push contour 
    for (int k = 0; k < image.cols; k++)
        for (int i = 0; i < image.rows; i++)
        {
            if (int(image.at<uchar>(i, k)) != 0)
            {
                con[0].push_back(Point(k, i));
                break;
            }
        }
    for (int k = 0; k < image.cols; k++)
        for (int i = image.rows - 1; i >= 0; i--)
        {
            if (int(image.at<uchar>(i, k)) != 0)
            {
                con[1].push_back(Point(k, i));
                break;
            }
        }

    Point2f data[2];
    double slope[2];
    for (int i = 0; i < 2; i++)
    {
        Vec4f fitline;
        fitLine(con[i], fitline, 2, 0, 1e-2, 1e-2);			   // contours fit line -> fitline ( vx , vy , x0, y0);
        double m = fitline[1] / fitline[0];
        Point2f pt1(0, m * (0 - fitline[2]) + fitline[3]);						   // 點斜式 (y2-y1)/(x2-x1) = m                                                                                                                                                           
        Point2f pt2(image.cols - 1, m * (image.cols - 1 - fitline[2]) + fitline[3]); // x = end point
        line(result, pt1, pt2, 255, 1);
        slope[i] = m;
        data[i] = pt1;
    }
    cout << fixed << setprecision(4) << cal_len(data, slope, image); // 一條斜率m的線 取其與兩線交點 並算距離

}
double cal_len(Point2f p[], double slope[], Mat image)//POGCHAMP　哈　文盲
{
    double m = tan((90 - (deg(slope[0]) + deg(slope[1])) / 2) * CV_PI / 180);
    double c0 = image.rows / 2 - m * image.cols / 2; // y = mx + c0 斜截式 主線
    double c[2] = { p[0].y - slope[0] * p[0].x, p[1].y - slope[1] * p[1].x };
    Point2f pt[2];
    for (int i = 0; i < 2; i++)
    {
        double x = (c0 - c[i]) / (m - slope[i]); // (c-b)/(m-m2) = x
        double y = m * x + c0;				     // y = mx + b
        pt[i] = Point2f(x, y);
    }
    return sqrt((pt[1].x - pt[0].x) * (pt[1].x - pt[0].x) + (pt[1].y - pt[0].y) * (pt[1].y - pt[0].y));

}