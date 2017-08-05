//============================================================================
// Name        : transistor_vision.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Extract objects and their features from a playing game
//============================================================================

#include<iostream>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/opencv.hpp>
#include<iostream>
#include<vector>
#include <algorithm>

#include "yingyang.h"
#include "SeperateObjects.h"
#include "featureextraction.h"


using namespace std;
using namespace cv;

int vision_analysis()
{
	//system("/home/sheun/Gaming_Project/game_vision/gstream_command_to_capture_image");
		//get image
		Mat img = imread("/home/sheun/Pictures/transistor_images/transistor1.jpg");

		//grayscale, and use imadjust for to get a high constrast version (the base for "lightworld")
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

		imshow("dark world view", dark_world_view);
		imshow("light world view", light_world_view);

		//get objects in each world view
		SeperateObjects worldObjects;
		vector <Mat> dark_world_objects  = worldObjects.BoundBox(dark_world_view, gray,Original_image_clone, 0); // the 2nd parameter is because we want the boxes to be on the original image
		vector <Mat> light_world_objects = worldObjects.BoundBox(light_world_view, gray,Original_image_clone, 1);

		feature_extraction features_of_objects;
		vector< vector<KeyPoint> > features_of_dark_world_objects = features_of_objects.featurePoints(dark_world_objects,0);
		vector< vector<KeyPoint> > features_of_light_world_objects = features_of_objects.featurePoints(light_world_objects,1);


		cout<<features_of_light_world_objects.size()<<endl;
		cout<<features_of_dark_world_objects.size()<<endl;
		namedWindow( "Objects in both worlds", CV_WINDOW_NORMAL );
		imshow ("Objects in both worlds",Original_image_clone);

		//cvWaitKey();


		return 0;
}
int main()
{
	return vision_analysis();

}


#include <boost/python.hpp>
using namespace boost::python;
BOOST_PYTHON_MODULE(main)
{

    def("vision", vision_analysis);
}
