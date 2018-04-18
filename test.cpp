#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <unistd.h>
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

/** Function Headers */
void detectAndDisplay( Mat frame );


/** Global variables */
int TIME_COUNTER = 0;
int x = 0;
int y = 0;
int w = 0;
int h = 0;
int area = 0;

//for Eyes
int k_wakeUp = 1;
int eyeClosedCounter = 0;
int EYE_CLOSED_FREAMES = 30;

//for Head size
int head_size = 0;
int head_size_counter = 0;
int updated_head_size = 0;
int current_head_pose_y = 0;
int head_counter_y = 0;

//for Head profile
int head_counter_right = 0;
int head_counter_left = 0;


String face_cascade_name = "/home/luka/opencv-3.1.0/data/haarcascades/haarcascade_frontalface_default.xml";
String eyes_cascade_name = "/home/luka/opencv-3.1.0/data/haarcascades/haarcascade_eye.xml";
String profile_cascade_name = "17har.xml";


CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
CascadeClassifier profile_cascade;

string window_name = "Capture - Face detection";

/** main */
int main( int argc, const char** argv )
{
//VideoCapture capture("vid.mkv");
//VideoCapture capture("Video1.wmv");
VideoCapture capture("Video2.wmv");
//VideoCapture capture("Video3.wmv");
//VideoCapture capture("Video4.wmv");
//VideoCapture capture("Video5.wmv");
Mat frame;

   //-- 1. Load the cascades
	if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading face cascade\n"); return -1; };
	if( !eyes_cascade.load( eyes_cascade_name ) ){ printf("--(!)Error loading eyes cascade\n"); return -1; };
	if( !profile_cascade.load( profile_cascade_name ) ){ printf("--(!)Error loading profile face cascade\n"); return -1; };
	
   //-- 2. Read the video stream
   if (capture.isOpened())
   {
     while( true )
     {
		 
	//resizeWindow("frame",600,450);
    capture.read(frame);

   //-- 3. Apply the classifier to the frame
       if( !frame.empty() )
       { 
		   detectAndDisplay( frame ); 
		}
       else
       { 
		   printf(" --(!) No captured frame -- Break!"); 
		   break; 
		}
       int c = waitKey(10);
       
       if( (char)c == 'c' ) { break; }
      }
   }
   return 0;
 }
 
 void detectAndDisplay( Mat frame )
{
  std::vector<Rect> faces;
  cv::Rect maxRect;
  std::vector<Rect> profileFace;
  Mat frame_gray;

  cvtColor( frame, frame_gray, CV_BGR2GRAY );


  // -- Detect faces
  face_cascade.detectMultiScale( frame_gray, faces, 1.2, 8);
	// ...
	//for (std::vector<Rect>::const_iterator i = faces.begin(); i != faces.end(); ++i)
    //std::cout << *i << ' ';
 
  // -- If face is detected
  if(faces.size() > 0) {
	  head_counter_right = 0;
	  head_counter_left = 0;
	  
	  k_wakeUp = 1;
	  for( size_t i = 0; i < faces.size(); i++ )
	  {
		x = faces[i].x;
		y = faces[i].y;
		w = faces[i].width;
		h = faces[i].height;
		
		if(faces[i].area() > maxRect.area()) {
			//printf("Uso u face\n");
			maxRect = faces[i];
			}
		}
		if(maxRect.x != 0) {
			x = maxRect.x;
			y = maxRect.y;
			w = maxRect.width;
			h = maxRect.height;
		
			rectangle( frame, Point(x,y), Point(x + w,y + h),Scalar( 255, 0, 0 ), 2);
			
			Mat faceROIgray = frame_gray( maxRect );
			Mat faceROIcolor = frame(maxRect);
			std::vector<Rect> eyes;
			
			//-- Set starting points
			if(TIME_COUNTER == 0) {
				head_size = w*h;
				current_head_pose_y = y;
				TIME_COUNTER += 1;
				printf("Starting head pose y axis: %d\n",y);
				printf("Pocetna povrsina: %d\n", head_size);
				sleep(5);
			}

			//-- In each face, detect eyes
			eyes_cascade.detectMultiScale( faceROIgray, eyes, 1.3, 20);

			for( size_t j = 0; j < eyes.size(); j++ )
			 {	k_wakeUp = 0;
				int ex = eyes[j].x;
				int ey = eyes[j].y;
				int ew = eyes[j].width;
				int eh = eyes[j].height;
				rectangle( faceROIcolor, Point(ex,ey), Point(ex + ew,ey + eh),Scalar( 255, 255, 0 ), 2);
			 }
		  }	
	  
	  updated_head_size = w*h;
	  //printf("Pocetna povrsina: %d\n", updated_head_size);
	  
	  // -- Checking Eyes closed
	  if(k_wakeUp == 1) {
		  eyeClosedCounter += 1;
		  if(eyeClosedCounter >= EYE_CLOSED_FREAMES){
			  putText(frame,"WAKE UP", Point(10,85), 2, 0.7, Scalar( 0, 0, 255 ) ,2);
		  }
	  } else {
		  eyeClosedCounter = 0;
	  }
	  // -- Checking Head size
	  if(updated_head_size > head_size*1.3 || updated_head_size < head_size*0.70) {
		  head_size_counter += 1;
		  if(head_size_counter > 10) {
			  if(updated_head_size > head_size*1.3 || updated_head_size < head_size*0.70){
				  head_size = w*h;
				  current_head_pose_y = y;
				  printf("Starting head pose y axis u chz: %d\n",current_head_pose_y);
				  printf("Pocetna povrsina: %d\n", head_size);
				  }
			  }
		  
		  
	  } else {
		  if(current_head_pose_y*1.1 < y) {
			  
			  head_counter_y += 1;
			  
			  if(head_counter_y > 20) {
				  printf("Head pose y: %d\n",current_head_pose_y);
				  if(current_head_pose_y*1.1 < y) {
					  putText(frame,"HEAD DOWN!", Point(10,30), 2, 0.7, Scalar( 0, 0, 255 ) ,2);
					  }
				  }
				  
			} else {
				  head_counter_y = 0;
				  }
		  }
  } else {
	  profile_cascade.detectMultiScale( frame_gray, profileFace, 1.1, 2);
	  if(profileFace.size() > 0) {
		head_counter_right = 0;	
		head_counter_left += 1;
		
		for( size_t i = 0; i < profileFace.size(); i++ )
		 {	
			int ex = profileFace[i].x;
			int ey = profileFace[i].y;
			int ew = profileFace[i].width;
			int eh = profileFace[i].height;
			rectangle(frame, Point(ex,ey), Point(ex + ew,ey + eh),Scalar( 255, 255, 0 ), 2);
		 }
		 if(head_counter_left > 20) {
			putText(frame,"TURN YOUR HEAD LEFT!", Point(300,30), 2, 0.7, Scalar( 0, 0, 255 ) ,2);
		 }
	  
	  
	  } else {
			head_counter_left = 0;
			head_counter_right += 1;
			flip(frame,frame,1);
			cvtColor( frame, frame_gray, CV_BGR2GRAY );
			profile_cascade.detectMultiScale( frame_gray, profileFace, 1.1, 2);
			for( size_t i = 0; i < profileFace.size(); i++ )
			{	
			int ex = profileFace[i].x;
			int ey = profileFace[i].y;
			int ew = profileFace[i].width;
			int eh = profileFace[i].height;
			rectangle(frame, Point(ex,ey), Point(ex + ew,ey + eh),Scalar( 255, 255, 0 ), 2);
			}
			flip(frame,frame,1);
			if(head_counter_right > 20 && profileFace.size() > 0) {
			putText(frame,"TURN YOUR HEAD RIGHT!", Point(300,30), 2, 0.7, Scalar( 0, 0, 255 ) ,2);
			}
	}
	  //printf("nema glave okrenuta\n");
  }
  
  //-- Show what you got
  imshow( window_name, frame );
 }

