#include<opencv2/opencv.hpp>
#include<iostream>
#include<string>
#include<vector>

using namespace std;
using namespace cv;

void PreProcess(Mat& img, Mat& mask, Mat& result)
{
    Scalar upper(253, 225, 255);
    Scalar lower(0, 0, 0);

    inRange(img, lower, upper, mask);// 取骨頭那圈
    bitwise_not(mask, mask);

    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    findContours(mask, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE); // 用mask畫框 , 在將選取塗滿 做MASK 
    vector<Rect> boundRect(contours.size());

    int Max = 0;
    for (int i = 0; i < contours.size(); i++)
    {
        drawContours(mask, contours, i, 255, -1);
        int Area = contourArea(contours[i]);
        int MArea = contourArea(contours[Max]);
        Max = (Area > MArea) ? i : Max;
        boundRect[i] = boundingRect(contours[i]);
    }

    for (int i = 0; i < contours.size(); i++)
        if (i != Max)
        {
            rectangle(mask, boundRect[i].tl(), boundRect[i].br(), 0, -1); // CT 輔助線 
            rectangle(img, boundRect[i].tl(), boundRect[i].br(), 0, -1);
        }
    img.copyTo(result, mask);//(cant use)bitwise_and  / copyTo  ?_?

}

void imgRotate(Mat& img, Mat mask)
{
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    findContours(mask, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    int Max = 0;
    for (int i = 0; i < contours.size(); i++)
    {
        int Area = contourArea(contours[i]);
        int MArea = contourArea(contours[Max]);
        Max = (Area > MArea) ? i : Max;
    }

    RotatedRect rect = fitEllipse(contours[Max]); // 原圖上找fitEllipse(橢圓 用來旋轉

    double tangle = rect.angle;
    if (tangle > 90)
        tangle = -180 + tangle;
    else if (tangle < -45)
        tangle += 90;
    Mat rotation_matrix = getRotationMatrix2D(rect.center, tangle, 1.0);

    warpAffine(img, img, rotation_matrix, img.size());

}

void func3(Mat img, Mat mask) // generate new mask
{
    vector<vector<Point>> contours;
    findContours(img, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    drawContours(mask, contours, 0, 255, -1);
}

int main(int argc, char* argv[])
{
    for (int i = 1; i < 9; i++) {
        String image_path = "HW1/Input/public_0" + to_string(i) + ".png";
        string save_path = "HW1/Output/o" + to_string(i) + ".jpg";
        Mat img = imread(image_path);
        cvtColor(img, img, COLOR_BGR2GRAY);
        Mat mask, result1;
        mask = Mat::zeros(img.size(), CV_8UC1);

        PreProcess(img, mask, result1);// result1 = after Process

        Mat mask2;
        mask2 = Mat::zeros(img.size(), CV_8UC1); // only black img
        Mat copy1 = result1.clone();
        func3(copy1, mask2); // mask2 complete

        imgRotate(result1, mask2);
        imwrite(save_path, result1);

    }
    

}
