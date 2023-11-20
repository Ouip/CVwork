#include<opencv2/opencv.hpp>
#include<iostream>
#include<string>
#include<vector>
#include<cmath>
#include<iomanip>

using namespace std;
using namespace cv;

double deg(double x)
{
    return atan(x) * 180.0 / CV_PI;
}
double DrawLine(Mat& image);
vector<Point> GetContour(Mat image, int height, int mode);
double Cal_Length(Point2f p[], double slope[], Mat image);

Mat TMP;

int main(int argc, char* argv[])
{
    for (int i = 1; i <= 10; i++) {
        String image_path = "HW2/Input/public" + to_string(i) + ".jpg";
        Mat img = imread(image_path);
        TMP = img.clone();
        cvtColor(img, img, COLOR_BGR2GRAY);
        cout << fixed << setprecision(4) << DrawLine(img);
        imwrite("HW2/Output/o"+to_string(i)+".jpg", TMP);
    }
    

}

double DrawLine(Mat& image)
{
    threshold(image, image, 128, 255, THRESH_BINARY);
    GaussianBlur(image, image, Size(3, 3), 0, 1);
    Canny(image, image, 20, 10);

    vector<Point> con[2];  // Contours
    con[0] = GetContour(image, 0, 1);				// upper 
    con[1] = GetContour(image, image.rows - 1, -1);	// lower

    Point2f data[2];
    double slope[2]={0};
    for (int i = 0; i < 2; i++)
    {
        Vec4f fitline;

        fitLine(con[i], fitline, 2, 0, 1e-2, 1e-2);									 // fitline ( vx , vy , x0, y0);
        double m = fitline[1] / fitline[0];

        Point2f pt1(0, m * (0 - fitline[2]) + fitline[3]);						     // (y2-y1)/(x2-x1) = m                                                                                                                                                           
        Point2f pt2(image.cols - 1, m * (image.cols - 1 - fitline[2]) + fitline[3]); // x = end point
        line(TMP,pt1,pt2,Scalar(255,0,0), 2);
        slope[i] = m;
        data[i] = pt1;
    }
    return Cal_Length(data, slope, image);
    
}

vector<Point> GetContour(Mat image, int height, int mode) // find contours height = start pos , mode => below 1 / above -1;
{
    vector<Point> con(image.cols);
    for (int k = 0; k < image.cols; k++)
        for (int i = height;; i += mode)
            if (image.at<uchar>(i, k))
            {
                con[k] = (Point(k, i));
                break;
            }
    return con;
}

double Cal_Length(Point2f p[], double slope[], Mat image)
{
    double m = tan((90 - (deg(slope[0]) + deg(slope[1])) / 2) * CV_PI / 180);	// rad
    double c0 = image.rows / 2.0 - m * image.cols / 2.0;						// y = mx + c0 
    double c[2] = { p[0].y - slope[0] * p[0].x, p[1].y - slope[1] * p[1].x };

    Point2f pt[2];
    
    for (int i = 0; i < 2; i++)
    {
        double x = (c[i] - c0) / (m - slope[i]);
        double y = m * x + c0;

        pt[i] = Point2f(x, y);
    }
    cout << pt[0] << " " << pt[1] << endl;
    line(TMP, pt[0], pt[1], Scalar(255, 0, 0), 2);
    return sqrt((pt[1].x - pt[0].x) * (pt[1].x - pt[0].x) + (pt[1].y - pt[0].y) * (pt[1].y - pt[0].y));
}
