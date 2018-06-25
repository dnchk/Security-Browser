#pragma once

#include<iostream>
#include<vector>
#include<chrono>
#include <atomic>

#include <opencv2\objdetect.hpp>

#include"Frame.h"

namespace ISXFaceDetector
{
class FaceDetector
{
public:
	FaceDetector(int num_of_chosen_camera = 0); // add algorithm, path for saving
	~FaceDetector() {};

	void RestartFaceDetector(int num_of_chosen_camera = 0);
	void StopFaceDetector();

	ISXFrame::Frame CaptureFrame();
	
	// algorithms for detecting suspicious behaviour and saving photos
	int SmartDetectAndSave(int test_duration_in_min = 40, int max_amount_of_photos = 50, int interval_in_sec_check_suspicious_behaviour = 5);
	void SaveWithInterval(int interval_in_sec = 5);
	void SaveConstantly();
	
	// final algorithm for a separate thread (doesn't open an output window)
	int Run(int test_duration_in_min = 40, int max_amount_of_photos = 50, int interval_in_sec_check_suspicious_behaviour = 5);

	// auxiliary functions
	void DrawRectAroundFaces(const std::vector<cv::Rect>& faces);
	void SaveZeroAndManyFaces(const std::vector<cv::Rect>& faces);
	bool PositionChanged(int x, int width, int y, int height, int delta = 15);

private:
	// stop face detector if (stop == true)
	std::atomic_bool stop = false;
	
	cv::VideoCapture camera;
	ISXFrame::Frame frame;
	cv::CascadeClassifier detector;
	const std::string CASCADE_FILE = "haarcascade_frontalface_alt.xml";
	
	//face position
	std::atomic_int x0 = 0;
	std::atomic_int x1 = 0;
	std::atomic_int y0 = 0;
	std::atomic_int y1 = 0;
};
}
