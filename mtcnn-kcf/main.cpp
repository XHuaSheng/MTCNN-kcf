#include "mtcnn.h"
#include <opencv2/opencv.hpp>

#include "kcftracker.hpp"
using namespace cv;
using namespace std;

#define MAXFACEOPEN 0 //�����Ƿ񿪹�����������ԣ�1Ϊ��������Ϊ��



void test_video() {
	char *model_path = "/home/xckj/software/ncnn-master/build/models-face";
	MTCNN mtcnn(model_path);
	mtcnn.SetMinFace(40);
	cv::VideoCapture mVideoCapture(0);
	if (!mVideoCapture.isOpened()) {
		return;
	}
	cv::Mat frame;
	mVideoCapture >> frame;
	while (!frame.empty()) {
		mVideoCapture >> frame;
		if (frame.empty()) {
			break;
		}

		clock_t start_time = clock();
		
		ncnn::Mat ncnn_img = ncnn::Mat::from_pixels(frame.data, ncnn::Mat::PIXEL_BGR2RGB, frame.cols, frame.rows);
		std::vector<Bbox> finalBbox;
#if(MAXFACEOPEN==1)
		mtcnn.detectMaxFace(ncnn_img, finalBbox);
#else
		mtcnn.detect(ncnn_img, finalBbox);
#endif
		const int num_box = finalBbox.size();
		std::vector<cv::Rect> bbox;
		bbox.resize(num_box);
		for(int i = 0; i < num_box; i++){
			bbox[i] = cv::Rect(finalBbox[i].x1, finalBbox[i].y1, finalBbox[i].x2 - finalBbox[i].x1 + 1, finalBbox[i].y2 - finalBbox[i].y1 + 1);
		
			for (int j = 0; j<5; j = j + 1)
			{
				cv::circle(frame, cvPoint(finalBbox[i].ppoint[j], finalBbox[i].ppoint[j + 5]), 2, CV_RGB(0, 255, 0), CV_FILLED);
			}
		}
		for (vector<cv::Rect>::iterator it = bbox.begin(); it != bbox.end(); it++) {
			rectangle(frame, (*it), Scalar(0, 0, 255), 2, 8, 0);
		}
		imshow("face_detection", frame);
		clock_t finish_time = clock();
		double total_time = (double)(finish_time - start_time) / CLOCKS_PER_SEC;
		std::cout << "time" << total_time * 1000 << "ms" << std::endl;
	
		int q = cv::waitKey(10);
		if (q == 27) {
			break;
		}
	}
	return ;
}

int test_picture(MTCNN &mtcnn, std::vector<cv::Rect> &bbox, Mat image ){
	clock_t start_time = clock();

	//cv::Mat image;
	//image = cv::imread("/home/xckj/software/ncnn-master/build/sample.jpg");
	ncnn::Mat ncnn_img = ncnn::Mat::from_pixels(image.data, ncnn::Mat::PIXEL_BGR2RGB, image.cols, image.rows);
	std::vector<Bbox> finalBbox;
	//std::vector<cv::Rect> bbox;
#if(MAXFACEOPEN==1)
	mtcnn.detectMaxFace(ncnn_img, finalBbox);
#else
	mtcnn.detect(ncnn_img, finalBbox);
#endif

	const int num_box = finalBbox.size();
	bbox.resize(num_box);
	for (int i = 0; i < num_box; i++) {
		bbox[i] = cv::Rect(finalBbox[i].x1, finalBbox[i].y1, finalBbox[i].x2 - finalBbox[i].x1 + 1, finalBbox[i].y2 - finalBbox[i].y1 + 1);

		for (int j = 0; j<5; j = j + 1)
		{
			cv::circle(image, cvPoint(finalBbox[i].ppoint[j], finalBbox[i].ppoint[j + 5]), 2, CV_RGB(0, 255, 0), CV_FILLED);
		}
	}
	for (vector<cv::Rect>::iterator it = bbox.begin(); it != bbox.end(); it++) {
		rectangle(image, (*it), Scalar(0, 0, 255), 2, 8, 0);
	}

	imshow("face_detection", image);
	clock_t finish_time = clock();
	double total_time = (double)(finish_time - start_time) / CLOCKS_PER_SEC;
	std::cout << "time  " << total_time * 1000 << "ms" << std::endl;

	cv::waitKey(1);

}

int main(int argc, char** argv) {
	
	bool HOG = true;
	bool FIXEDWINDOW = false;
	bool MULTISCALE = true;
	bool SILENT = true;
	bool LAB = false;
	Rect result;
	//test_video();
	std::vector<Rect>  bbox;

	char *model_path = "/home/xckj/software/ncnn-master/build/models-face";
	MTCNN mtcnn(model_path);
	// Create the KCFTracker object with one of the available options
	KCFTracker tracker(HOG, FIXEDWINDOW, MULTISCALE, LAB);
	cv::VideoCapture mVideoCapture(0);
	if (!mVideoCapture.isOpened()) {
		return 0;
	}
	cv::Mat frame;
	int count = 0;
	float peak_value = 0;
	mVideoCapture >> frame;
	while (!frame.empty()) 
	{
		mVideoCapture >> frame;
		if (frame.empty()) 
		{
			break;
		}
		clock_t start_time = clock();
		if(bbox.empty())
		{
			test_picture(mtcnn, bbox, frame);
			if(bbox.size())
				tracker.init( Rect(bbox[0].x,bbox[0].y,bbox[0].width,bbox[0].height), frame );
		}
		if(!bbox.empty()&&peak_value< 0.5 )
		{
			test_picture(mtcnn, bbox, frame);
			tracker.init( Rect(bbox[0].x,bbox[0].y,bbox[0].width,bbox[0].height), frame );
			count = 1;
		}
		
		

		if(bbox.size())
		{
			// for(int i = 0;i<bbox.size();i++)
			// {
				// Give the first frame and the position of the object to the tracker
				// Get the position of the object for the new frame
				result = tracker.update(frame, peak_value);
				cout<<peak_value<<endl;
				rectangle( frame, Point( result.x, result.y ), Point( result.x+result.width, result.y+result.height), Scalar( 0, 255, 255 ), 1, 8 );
			//}
		}
		clock_t finish_time = clock();
		double total_time = (double)(finish_time - start_time) / CLOCKS_PER_SEC;
		std::cout << "time  " << total_time * 1000 << "ms" << std::endl;
		imshow("track", frame);
		waitKey(1);
		count++;
	}
	return 0;
}
