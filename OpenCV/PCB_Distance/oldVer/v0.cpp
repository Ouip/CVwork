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
void PreProcess(Mat& image, Mat& result)
{
    vector<vector<Point>> contours;
    priority_queue<pair<int, int>> pq;

    threshold(image, image, 128, 255, THRESH_BINARY);
    GaussianBlur(image, image, Size(3, 3), 0, 1);
    Canny(image, image, 20, 10);
    GaussianBlur(image, image, Size(3, 3), 0, 1);
    erode(image, image, Mat());

    findContours(image, contours, RETR_LIST, CHAIN_APPROX_NONE);

    for (int i = 0; i < contours.size(); i++)
        pq.push(make_pair(arcLength(contours[i], true), i));
    Vec4f data[2];
    double slope[2];
    for (int i = 0; i < 2; i++)
    {
        Vec4f fitline;
        int t = pq.top().second;
        pq.pop();
        fitLine(contours[t], fitline, 2, 0, 1e-2, 1e-2);			   // contours fit line -> fitline ( vx , vy , x0, y0);
        double m = fitline[1] / fitline[0];
        Point2f pt1(0, m * (0 - fitline[2]) + fitline[3]);						   // 헕그─ (y2-y1)/(x2-x1) = m                                                                                                                                                           
        Point2f pt2(image.cols - 1, m * (image.cols - 1 - fitline[2]) + fitline[3]); // x = end point
        slope[i] = m;
        data[i] = fitline;
        line(result, pt1, pt2, 255, 1);

    }

    Point2f pt1, pt2;
    pt1.x = pt2.x = ceil(image.cols / 2.0);

    //그쾤 vec4f 
    double m1 = data[0][1] / data[0][0];
    double m2 = data[1][1] / data[1][0];

    pt1.y = m1 * (pt1.x - data[0][2]) + data[0][3];
    pt2.y = m2 * (pt2.x - data[1][2]) + data[1][3];

    double m = tan((90 - (deg(slope[0]) + deg(slope[1])) / 2) * CV_PI / 180);
    double angle = atan(m) * 180.0 / CV_PI;

    Mat M = getRotationMatrix2D(Point2f(image.cols / 2, image.rows / 2), 90 - angle, 1);
    pt1.x = M.at<double>(0, 0) * pt1.x + M.at<double>(0, 1) * pt1.y + M.at<double>(0, 2);
    pt1.y = M.at<double>(1, 0) * pt1.x + M.at<double>(1, 1) * pt1.y + M.at<double>(1, 2);

    pt2.x = M.at<double>(0, 0) * pt2.x + M.at<double>(0, 1) * pt2.y + M.at<double>(0, 2);
    pt2.y = M.at<double>(1, 0) * pt2.x + M.at<double>(1, 1) * pt2.y + M.at<double>(1, 2);
    cout << fixed << setprecision(4) << sqrt((pt2.x - pt1.x) * (pt2.x - pt1.x) + (pt2.y - pt1.y) * (pt2.y - pt1.y));
    imshow("1", result);
    waitKey(0);

}

int main(int argc, char* argv[])
{
    String image_path = argv[1];
    for (int i = 1; i < 11; i++)
    {
        image_path = "public/public" + to_string(6) + ".jpg";
        Mat img = imread(image_path);
        Mat result = img.clone();
        cvtColor(img, img, COLOR_BGR2GRAY);
        PreProcess(img, result);

        cout << endl;

    }

}