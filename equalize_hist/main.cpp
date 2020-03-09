#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int gray[256] = { 0 };          //��¼ÿ���Ҷȼ����µ����ظ���
double gray_prob[256] = { 0 };  //��¼�Ҷȷֲ��ܶ�
double gray_distribution[256] = { 0 };  //��¼�ۼ��ܶ�
int gray_equal[256] = { 0 };    //���⻯��ĻҶ�
int gray_sum = 0;  //��������
const char* result_image = "ֱ��ͼ���⻯ֱ��ͼ";
const char* src_image = "ԭͼֱ��ͼ";

void show_histogram(Mat& img,const char* s);
Mat equalize_hist(Mat& input);


int main(int argc,char** argv)
{
    Mat result;
    Mat src = imread("D:/CODE/matlabku/�ô�ͼ��.jpg", IMREAD_GRAYSCALE);//ֱ�Ӷ��ɻҶ�ͼ
    if (src.empty()) {
        printf("could not load image...\n");
        return -1;
    }
    namedWindow("input image", CV_WINDOW_AUTOSIZE);

    imshow("input image", src);
    show_histogram(src,src_image);        //��ʾֱ��ͼ
    result = equalize_hist(src);            //����ֱ��ͼ���⻯
    imshow("equalize_hist result:", result);
    show_histogram(result, result_image);

    waitKey(0);
    return 0;
}
//ֱ��ͼ���⻯
Mat equalize_hist(Mat& input)
{
    Mat output = input.clone();
    gray_sum = input.cols * input.rows;     //��ȡ��������

    //ͳ��ÿ���Ҷ��µ����ظ���
    for (int i = 0; i < input.rows; i++)
    {
        uchar* p = input.ptr<uchar>(i);
        for (int j = 0; j < input.cols; j++)
        {
            int vaule = p[j];
            gray[vaule]++;
        }
    }

    //ͳ�ƻҶ�Ƶ��
    for (int i = 0; i < 256; i++)
    {
        gray_prob[i] = ((double)gray[i] / gray_sum);    
    }

    //�����ۼ��ܶ�
    gray_distribution[0] = gray_prob[0];  //0û���ۼ�
    for (int i = 1; i < 256; i++)
    {
        gray_distribution[i] = gray_distribution[i - 1] + gray_prob[i];
    }

    //���¼�����⻯��ĻҶ�ֵ���������롣ʹ�ù�ʽ��(N-1)*T+0.5
    for (int i = 0; i < 256; i++)
    {
        gray_equal[i] = (uchar)(255 * gray_distribution[i] + 0.5);
    }

    //ֱ��ͼ���⻯,����ԭͼÿ���������ֵ
    for (int i = 0; i < output.rows; i++)
    {
        uchar* p = output.ptr<uchar>(i);    //p����Ϊÿ��Ԫ�ص�ָ��
        for (int j = 0; j < output.cols; j++)
        {
            p[j] = gray_equal[p[j]];
        }
    }

    return output;
}

//��ʾ ֱ��ͼ
void show_histogram(Mat& img, const char* s)
{
    //Ϊ����ֱ��ͼ���ñ���  
    //��������Ҫ�����ͼ���ͨ����������Ҫ����ͼ����ĸ�ͨ����bgr�ռ���Ҫȷ������ b��g��r�ռ䣩  
    int channels = 0;
    //Ȼ������������Ľ���洢�� �ռ� ����MatND�������洢���  
    MatND dstHist;
    //��������ֱ��ͼ��ÿһ��ά�ȵ� ��������Ŀ�����ǽ���ֵ���飬���ж����飩  
    int histSize[] = { 256 };       //�������д��int histSize = 256;   ��ô������ü���ֱ��ͼ�ĺ�����ʱ�򣬸ñ�����Ҫд &histSize  
    //�����ȷ��ÿ��ά�ȵ�ȡֵ��Χ�����Ǻ����������  
    //���ȵö���һ�����������洢 ����ά�ȵ� ��ֵ��ȡֵ��Χ  
    float midRanges[] = { 0, 256 };
    const float* ranges[] = { midRanges };

    calcHist(&img, 1, &channels, Mat(), dstHist, 1, histSize, ranges, true, false);

    //calcHist  �������ý�����dstHist�����н������� ֱ��ͼ����Ϣ  ��dstHist��ģ�溯�� at<Type>(i)�õ���i��������ֵ  
    //at<Type>(i, j)�õ���i�����ҵ�j��������ֵ  

    //��ʼֱ�۵���ʾֱ��ͼ��������ֱ��ͼ  
    //�����ȴ���һ���ڵ׵�ͼ��Ϊ�˿�����ʾ��ɫ�����Ըû���ͼ����һ��8λ��3ͨ��ͼ��  
    Mat drawImage = Mat::zeros(Size(256, 256), CV_8UC3);
    //��Ϊ�κ�һ��ͼ���ĳ�����ص��ܸ��������п��ܻ��кܶ࣬�ᳬ���������ͼ��ĳߴ磬�������������ȶԸ������з�Χ������  
    //���� minMaxLoc�������õ�����ֱ��ͼ������ص�������  
    double g_dHistMaxValue;
    minMaxLoc(dstHist, 0, &g_dHistMaxValue, 0, 0);
    //�����صĸ������ϵ� ͼ������Χ��  
    //����ֱ��ͼ�õ�������  
    for (int i = 0; i < 256; i++)
    {
        int value = cvRound(dstHist.at<float>(i) * 256 * 0.9 / g_dHistMaxValue);

        line(drawImage, Point(i, drawImage.rows - 1), Point(i, drawImage.rows - 1 - value), Scalar(255, 255, 255));
    }

    imshow(s, drawImage);
}