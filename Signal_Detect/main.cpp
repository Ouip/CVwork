#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

#define LOCAL

#define RedL Scalar(0, 100, 46)
#define RedH Scalar(10, 255, 255)
#define BlueL Scalar(100, 43, 46)
#define BlueH Scalar(124, 255, 255)
#define YellowL Scalar(20, 100, 100)
#define YellowH Scalar(40, 255, 255)


class Signal {
   public:
    Mat Image;
    using vecPoint = vector<Point>;
    Signal(string image_path) {
        _img = imread(image_path);
        Image = _img.clone();
    }

   public:
    void GetSignal() {
        // Three Color Mask
        ColorMask(_img);
        // Get boundRect Point
        Printer(DrawBound());
    }

    void ColorMask(Mat& img) {
        GaussianBlur(img, img, Size(9, 9), 2, 2);
        cvtColor(img, img, COLOR_BGR2HSV);

        Mat temp[2];
        inRange(img, Scalar(156, 50, 60), Scalar(180, 255, 255), temp[0]);
        inRange(img, RedL, RedH, temp[1]);
        mask[0] = temp[0] | temp[1];

        inRange(img, BlueL, BlueH, mask[1]);

        inRange(img, YellowL, YellowH, mask[2]);
    }

    vecPoint DrawBound() {
        vecPoint result;
        for (int i = 0; i < 3; ++i) {
            vector<vecPoint> contours;
            findContours(mask[i], contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

            for (auto& s : contours) {
                if (contourArea(s) < 1000)
                    continue;
                Rect boundRect = boundingRect(s);
                // TEST
                rectangle(Image, boundRect.tl(), boundRect.br(), 255, 1);
                circle(Image, boundRect.tl(), 3, 100, -1);
                circle(Image, boundRect.br(), 3, 100, -1);
                result.emplace_back(boundRect.tl());
                result.emplace_back(boundRect.br());
            }
            vector<vecPoint>().swap(contours);  // release
        }
        return result;
    }

    void Printer(vecPoint v) {
        for (int i = 0; i < v.size();) {
            cout << v[i].x << " " << v[i].y << " ";
            if (!(++i % 2))
                cout << endl;
        }
#ifdef LOCAL
        cout << "+-----------+" << endl;
#endif
        vecPoint().swap(v);  // release vector
    }

   protected:
    Mat mask[3];
    Mat _img;
};

int main(int argc, char* argv[]) {
    String image_path;
    string out_path;
#ifdef LOCAL
    for (int i = 2; i < 5; ++i) {
        image_path = "HW3/Input/public" + to_string(i) + ".jpg";
        out_path = "HW3/Output/o"+ to_string(i) + ".jpg";
        Signal solution(image_path);
        solution.GetSignal();
        imwrite(out_path,solution.Image);
    }
#else
    image_path = argv[1];
    Signal solution(image_path);
    solution.GetSignal();
#endif
}
