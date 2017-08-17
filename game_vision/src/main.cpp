//============================================================================
// Name        : transistor_vision.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Extract objects and their features from a playing game
//============================================================================
#include<stdio.h>
#include<iostream>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/opencv.hpp>
#include "opencv2/text/ocr.hpp"
#include<iostream>
#include<vector>
#include <algorithm>
#include <map>
#include <iterator>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

#include "yingyang.h"
#include "SeperateObjects.h"
#include "featureextraction.h"
#include "SendDataToPython.h"


using namespace std;
using namespace cv;

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>

using namespace boost::python;

boost::python::dict vision_analysis()
{
		//this map will hold all the information on the image such as its position in the frame and feature points
		typedef int object; //object number
		typedef std::pair<int, int > coordinates; //this will hold x,y coordinates of object in the frame
		typedef vector<KeyPoint> featurePoints; // feature points of object


		std::map <object, std::pair<coordinates, featurePoints > > dark_object_info;
		std::map <object, std::pair<coordinates, featurePoints > > light_object_info;
		//hold coordinates to be later inserted for dark contrast objects
		vector<int>dark_x_coordinate;
		vector<int>dark_y_coordinate;
		//hold coordinates to be later inserted for light contrast objects
		vector<int>light_x_coordinate;
		vector<int>light_y_coordinate;

		// turn on script that save get current frame from video game
	    //system("/home/sheun/Gaming_Project/game_vision/gstream_command_to_capture_image &");

	    //read current video_game frame
	    Mat img = imread("/home/sheun/Gaming_Project/game_vision/current_game_frame.jpg");

	    //grayscale, and use imadjust for to get a high contrast version (the basIS for "lightworld")
		Mat gray;
		//convert to grayscale
		cvtColor(img, gray, COLOR_RGB2GRAY);

		//smooth image
		blur(gray, gray, Size(3,3));

		Mat Original_image_clone = gray.clone();

		//convert to binary
		ying_yang world_view;
		Mat dark_world_view = world_view.binary(gray,img);
		Mat light_world_view = world_view.binary_Inverse(gray,img);

		//get objects in each world view (light and dark contrast images) and put each of them into a vector
		SeperateObjects worldObjects;
		vector <Mat> dark_world_objects  = worldObjects.BoundBox(dark_world_view, gray,Original_image_clone, 0, dark_x_coordinate, dark_y_coordinate, false); // the 3rd parameter holds the version of the frame image that the boxes will be drawn onto the boxes to be on the original image
		vector <Mat> light_world_objects = worldObjects.BoundBox(light_world_view, gray,Original_image_clone, 1, light_x_coordinate, light_y_coordinate, false);

		feature_extraction features_of_objects;
		vector< vector<KeyPoint> > features_of_dark_world_objects = features_of_objects.featurePoints(dark_world_objects,0, false);
		vector< vector<KeyPoint> > features_of_light_world_objects = features_of_objects.featurePoints(light_world_objects,1, false);

		//Append vectors so that all objects can be put into the python dictionary
		features_of_dark_world_objects.insert(features_of_dark_world_objects.end(), features_of_light_world_objects.begin(), features_of_light_world_objects.end());

		dark_x_coordinate.insert(dark_x_coordinate.end(), light_x_coordinate.begin(), light_x_coordinate.end());
		dark_y_coordinate.insert(dark_y_coordinate.end(), light_y_coordinate.begin(), light_y_coordinate.end());

		//send data of objects in image to python
		SendDataToPython python_features_of_objects;
		boost::python::dict send_to_python_the_dark_world = python_features_of_objects.objectInformationToDict(features_of_dark_world_objects, dark_x_coordinate, dark_y_coordinate);

		return send_to_python_the_dark_world;

}

int letter()
{
	Mat img = imread("/home/sheun/Pictures/transistor_images/transistor4.jpg");
	blur(img, img, Size(3,3));
	Mat gray, binary_img;
	cvtColor(img, gray, COLOR_RGB2GRAY);
	threshold(gray, binary_img,0.5,255,THRESH_BINARY| CV_THRESH_OTSU);
	//imshow("binary_img", binary_img);
	for (int a = 0; a<4; ++a)
	{
		for (int b=0;b<7;++b)
		{
			cout<<"oem: "<<a<<" psmode: "<<b<<endl;
			Ptr<cv::text::OCRTesseract> ocr =
				cv::text::OCRTesseract::create(NULL /*datapath*/, "eng" /*lang*/, NULL /*whitelist*/, a /*oem*/, b /*psmode*/);

			string output;
			vector<Rect>   boxes;
			vector<string> words;
			vector<float>  confidences;

			ocr->run(img, output, &boxes, &words, &confidences, 1);
			for (int i = 0; i<words.size(); i++)
			{
				cout<<words[i]<<endl;
			}

		}
	}
	cvWaitKey();
	return 0;

}

int main()
{

	//vision_analysis();
	letter();
	return 0;

}

BOOST_PYTHON_MODULE(main)
{

	//definition that out python program will call start and retrieve informatin from our c++ code.
	 def("vision", vision_analysis);

}
