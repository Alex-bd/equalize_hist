#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int gray[256] = { 0 };          //记录每个灰度级别下的像素个数
double gray_prob[256] = { 0 };  //记录灰度分布密度
double gray_distribution[256] = { 0 };  //记录累计密度
int gray_equal[256] = { 0 };    //均衡化后的灰度
int gray_sum = 0;  //像素总数
const char* result_image = "直方图均衡化直方图";
const char* src_image = "原图直方图";

void show_histogram(Mat& img,const char* s);
Mat equalize_hist(Mat& input);


int main(int argc,char** argv)
{
    Mat result;
    Mat src = imread("D:/CODE/matlabku/济大图标.jpg", IMREAD_GRAYSCALE);//直接读成灰度图
    if (src.empty()) {
        printf("could not load image...\n");
        return -1;
    }
    namedWindow("input image", CV_WINDOW_AUTOSIZE);

    imshow("input image", src);
    show_histogram(src,src_image);        //显示直方图
    result = equalize_hist(src);            //调用直方图均衡化
    imshow("equalize_hist result:", result);
    show_histogram(result, result_image);

    waitKey(0);
    return 0;
}
//直方图均衡化
Mat equalize_hist(Mat& input)
{
    Mat output = input.clone();
    gray_sum = input.cols * input.rows;     //获取像素总数

    //统计每个灰度下的像素个数
    for (int i = 0; i < input.rows; i++)
    {
        uchar* p = input.ptr<uchar>(i);
        for (int j = 0; j < input.cols; j++)
        {
            int vaule = p[j];
            gray[vaule]++;
        }
    }

    //统计灰度频率
    for (int i = 0; i < 256; i++)
    {
        gray_prob[i] = ((double)gray[i] / gray_sum);    
    }

    //计算累计密度
    gray_distribution[0] = gray_prob[0];  //0没有累计
    for (int i = 1; i < 256; i++)
    {
        gray_distribution[i] = gray_distribution[i - 1] + gray_prob[i];
    }

    //重新计算均衡化后的灰度值，四舍五入。使用公式：(N-1)*T+0.5
    for (int i = 0; i < 256; i++)
    {
        gray_equal[i] = (uchar)(255 * gray_distribution[i] + 0.5);
    }

    //直方图均衡化,更新原图每个点的像素值
    for (int i = 0; i < output.rows; i++)
    {
        uchar* p = output.ptr<uchar>(i);    //p依次为每行元素的指针
        for (int j = 0; j < output.cols; j++)
        {
            p[j] = gray_equal[p[j]];
        }
    }

    return output;
}

//显示 直方图
void show_histogram(Mat& img, const char* s)
{
    //为计算直方图配置变量  
    //首先是需要计算的图像的通道，就是需要计算图像的哪个通道（bgr空间需要确定计算 b或g或r空间）  
    int channels = 0;
    //然后是配置输出的结果存储的 空间 ，用MatND类型来存储结果  
    MatND dstHist;
    //接下来是直方图的每一个维度的 柱条的数目（就是将数值分组，共有多少组）  
    int histSize[] = { 256 };       //如果这里写成int histSize = 256;   那么下面调用计算直方图的函数的时候，该变量需要写 &histSize  
    //最后是确定每个维度的取值范围，就是横坐标的总数  
    //首先得定义一个变量用来存储 单个维度的 数值的取值范围  
    float midRanges[] = { 0, 256 };
    const float* ranges[] = { midRanges };

    calcHist(&img, 1, &channels, Mat(), dstHist, 1, histSize, ranges, true, false);

    //calcHist  函数调用结束后，dstHist变量中将储存了 直方图的信息  用dstHist的模版函数 at<Type>(i)得到第i个柱条的值  
    //at<Type>(i, j)得到第i个并且第j个柱条的值  

    //开始直观的显示直方图——绘制直方图  
    //首先先创建一个黑底的图像，为了可以显示彩色，所以该绘制图像是一个8位的3通道图像  
    Mat drawImage = Mat::zeros(Size(256, 256), CV_8UC3);
    //因为任何一个图像的某个像素的总个数，都有可能会有很多，会超出所定义的图像的尺寸，针对这种情况，先对个数进行范围的限制  
    //先用 minMaxLoc函数来得到计算直方图后的像素的最大个数  
    double g_dHistMaxValue;
    minMaxLoc(dstHist, 0, &g_dHistMaxValue, 0, 0);
    //将像素的个数整合到 图像的最大范围内  
    //遍历直方图得到的数据  
    for (int i = 0; i < 256; i++)
    {
        int value = cvRound(dstHist.at<float>(i) * 256 * 0.9 / g_dHistMaxValue);

        line(drawImage, Point(i, drawImage.rows - 1), Point(i, drawImage.rows - 1 - value), Scalar(255, 255, 255));
    }

    imshow(s, drawImage);
}