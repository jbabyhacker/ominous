/**
 * @mainpage Ominous API
 *
 * @section About
 *
 * Ominous utilizes OpenCV to track colored objects on
 * a black surface. Each color group corresponds to
 * controlling a specific instrument. Each groups contains
 * two objects, one cube and one sphere. The position of
 * the cube controls the rhythm while the sphere controls
 * the melody played on their corresponding instrument.
 *
 * @authors Jason Klein
 * @authors Alexander O'Brien
 *
 * @copyright This project is released under the MIT License.
 */

#include "MusicTable.hpp"
#include "SerialComm.hpp"
#include "mappings.hpp"
#include <iostream>
#include <vector>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <unistd.h>

#define EN_BLUR 		0x00000001
#define EN_CONTRAST 	0x00000010
#define EN_COLOR 		0x00000100
#define EN_THRESH		0x00001000
#define EN_ED			0x00010000
#define EN_CANNY		0x00100000
#define EN_POLY			0x01000000

#define CROP_X_POS 105
#define CROP_WIDTH 1115
#define CROP_Y_POS 0
#define CROP_HEIGHT 720

using namespace std;
using namespace cv;

int blurControl = 2;
float contrastAlphaControl = 0.8f;
int contrastBetaControl = 0;
int lowHueControl = 18;
int lowSaturationControl = 21;
int lowValueControl = 73;
int highHueControl = 175;
int highSaturationControl = 210;
int highValueControl = 199;
int thresholdControl = 57;
int erodeDilateControl = 4;
int cannyT1Control = 5000;
int cannyT2Control = 5000;
int approxPolyControl = 10;

volatile int data[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
pthread_mutex_t dataMutex = PTHREAD_MUTEX_INITIALIZER;

int main(void) {
	cout << "Launching Mable..." << endl;

	VideoCapture cap(0); //capture the video from web cam

	if (!cap.isOpened())  // if not success, exit program
	{
		cout << "Unable to open web cam" << endl;
		return -1;
	}

	initGuiControl(true, EN_BLUR);

	while (1) {
		/*--------------------------------------------*/
		/*-- Read in source video stream -------------*/
		/*--------------------------------------------*/
		Mat src;
		bool bSuccess = cap.read(src); // read a new frame from video

		if (!bSuccess) { //if not success, break loop
			cout << "Cannot read a frame from video stream" << endl;
			break;
		}

		Mat thr, gray, hsv, mask, colorFilterd, res, resout, contrast, sat;
		/* -> resize -> */
		resize(src, src, Size(1280, 720));

		/* -> crop -> */
		Rect cropSrc(CROP_X_POS, CROP_Y_POS, CROP_WIDTH, CROP_HEIGHT);
		src = src(cropSrc);

		/*--------------------------------------------*/
		/*   Blur image frame                         */
		/*____________________________________________*/
		int bVal = blurControl + 1;
		/* -> Blur -> */
		blur(src, src, Size(bVal, bVal));

		/*--------------------------------------------*/
		/*   Contrast image frame                     */
		/*____________________________________________*/
		/* src -> contrast -> contrast */
		src.convertTo(contrast, -1, contrastAlphaControl, contrastBetaControl);

		/* BGR -> HSV */
		cvtColor(contrast, hsv, CV_BGR2HSV);

		/*--------------------------------------------*/
		/*   Color filter                             */
		/*____________________________________________*/
		Scalar low(lowHueControl, lowSaturationControl, lowValueControl);
		Scalar high(highHueControl, highSaturationControl, highValueControl);

		inRange(hsv, low, high, mask); //Threshold the image
		bitwise_and(hsv, hsv, colorFilterd, mask);

		/* HSV -> BGR */
		cvtColor(colorFilterd, colorFilterd, CV_HSV2BGR);
		/* BGR -> Gray */
		cvtColor(colorFilterd, gray, CV_BGR2GRAY);

		/*--------------------------------------------*/
		/*   Threshold                                */
		/*____________________________________________*/
		threshold(gray, res, thresholdControl, 255, 0);

		/*--------------------------------------------*/
		/*   Erode & Dialate                          */
		/*____________________________________________*/
		int edVal = erodeDilateControl + 1;
		//morphological opening (remove small objects from the foreground)
		erode(res, resout,
				getStructuringElement(MORPH_ELLIPSE, Size(edVal, edVal)));
		dilate(resout, resout,
				getStructuringElement(MORPH_ELLIPSE, Size(edVal, edVal)));

		//morphological closing (fill small holes in the foreground)
		dilate(resout, resout,
				getStructuringElement(MORPH_ELLIPSE, Size(edVal, edVal)));
		erode(resout, resout,
				getStructuringElement(MORPH_ELLIPSE, Size(edVal, edVal)));

		/*--------------------------------------------*/
		/*   Canny                                    */
		/*____________________________________________*/
		Canny(resout, thr, cannyT1Control, cannyT2Control, 5, false);
		vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;
		findContours(thr.clone(), contours, hierarchy, CV_RETR_EXTERNAL,
				CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

		vector<TrackedObject> trackedObjects;
		vector<vector<Point> > contours_poly(contours.size());
		vector<Rect> boundRect(contours.size());
		vector<float> contArea(contours.size());
		vector<Point2f> center(contours.size());
		vector<float> radius(contours.size());
		vector<vector<Point> > hull(contours.size());
		int apVal = approxPolyControl + 1;

		for (int i = 0; i < contours.size(); i++) {
			approxPolyDP(Mat(contours[i]), contours_poly[i], apVal, true);
			contArea[i] = contourArea(Mat(contours_poly[i]));
			boundRect[i] = boundingRect(Mat(contours_poly[i]));
			minEnclosingCircle((Mat) contours_poly[i], center[i], radius[i]);
			convexHull(Mat(contours[i]), hull[i], false);

			// polygons with four sides
			if (contours_poly[i].size() == 4) {
				//crop the blue square and show it by itself
				Mat crop;
				Rect sqCrop(boundRect[i]);
				//src.convertTo(sat, -1, 2.0f, 0);
				crop = hsv(sqCrop);

				Scalar m = mean(crop);

//				cout << m << endl;

				TrackedObject tObj = { '-', '-', Point2f(-1, -1) };
				Scalar tObjColor(255, 255, 255);

				// detect whether object color is blue, green, or red
				if (m.val[0] >= 70.0 && m.val[0] <= 120.0) {
					tObj.color = 'B'; // Blue
					tObjColor = Scalar(255, 0, 0);
				} else if (m.val[0] >= 20.0 && m.val[0] <= 70.0) {
					tObj.color = 'G'; // Green
					tObjColor = Scalar(0, 255, 0);
				} else if (m.val[0] <= 20.0 || m.val[0] >= 120.0) {
					tObj.color = 'R'; // Red
					tObjColor = Scalar(0, 0, 255);
				}

				// a color must be found
				// then detect object shape
				if (tObj.color != '-') {
					tObj.position = center[i];

					// detect whether object is a sphere (circle) or cube (rectangle)
					if (contArea[i] < 1200) {
						tObj.shape = 'c'; // circle
						tObj.zone = pointToZoneCircle(center[i], cropSrc);
						circle(src, center[i], (int) radius[i], tObjColor, 2, 8,
								0);
					} else {
						tObj.shape = 'r'; // rectangle
						tObj.zone = pointToZoneRect(center[i], cropSrc);
						rectangle(src, boundRect[i].tl(), boundRect[i].br(),
								tObjColor, 2, 8, 0);
					}

					// add found objects to a vector
					trackedObjects.push_back(tObj);
				}

				pthread_mutex_lock(&dataMutex);

				if (tObj.color == 'B' && tObj.shape == 'r') {
					data[0] += tObj.zone;
					data[6]++;
				} else if (tObj.color == 'B' && tObj.shape == 'c') {
					data[1] += tObj.zone;
					data[7]++;
				} else if (tObj.color == 'G' && tObj.shape == 'r') {
					data[2] += tObj.zone;
					data[8]++;
				} else if (tObj.color == 'G' && tObj.shape == 'c') {
					data[3] += tObj.zone;
					data[9]++;
				} else if (tObj.color == 'R' && tObj.shape == 'r') {
					data[4] += tObj.zone;
					data[10]++;
				} else if (tObj.color == 'R' && tObj.shape == 'c') {
					data[5] += tObj.zone;
					data[11]++;
				}

//				cout << "TrackedObject: " << tObj.position << endl;

//				for (int i = 0; i < 12; i++) {
//					cout << data[i] << ",";
//				}
//				cout << endl;

				pthread_mutex_unlock(&dataMutex);

#ifdef CALC_MMM
				objCounter++;

				for (int i = 0; i < 3; i++) {
					if (m.val[i] < sMin[i]) {
						sMin[i] = m.val[i];
					}

					if (m.val[i] > sMax[i]) {
						sMax[i] = m.val[i];
					}

					sSum[i] += m.val[i];
				}

				if (counter == 100) {
					cout << "Min: (";
					for (int i = 0; i < 3; i++) {
						cout << sMin[i];
						if (i != 2) {
							cout << ", ";
						} else {
							cout << ") ";
						}
					}

					cout << "Max: (";
					for (int i = 0; i < 3; i++) {
						cout << sMax[i];
						if (i != 2) {
							cout << ", ";
						} else {
							cout << ") ";
						}
					}

					cout << "Avg: (";
					for (int i = 0; i < 3; i++) {
						cout << sSum[i] / objCounter;
						if (i != 2) {
							cout << ", ";
						} else {
							cout << ") ";
						}
					}

					cout << endl;

					run = false;
				}
#endif
			} else {
//				cout << "Other: " << contours_poly[i].size() << endl;
			}
		}

#ifdef DEBUG

#endif

		imshow("src", src);
//		imshow("sat", sat);
//		imshow("contrast", contrast);
//		imshow("hsv", hsv);
//		imshow("mask", mask);
//		imshow("colorFiltered", colorFilterd);
//		imshow("gray", gray);
//		imshow("res", res);
//		imshow("resout", resout);
//		imshow("canny", thr);

		if (waitKey(1) == 27) {
			cout << "Esc key pressed. Exiting..." << endl;
			break;
		}
	}

	return 0;
}

/**
 * Creates sliderbar GUIs to control filtering elements
 *
 * @param group selects if trackbars are grouped
 * together into a single window
 * @param enable selects trackbars to be enabled (ex: EN_BLUR | EN_POLY)
 */
void initGuiControl(bool group, char enable) {

	string groupWin = "Control Panel";

	string windowNames[] = { "Blur Control", "Contrast Control", "Low Control",
			"High Control", "Threshold Control", "Erode & Dilate Control",
			"Canny Control", "Approx Poly Control" };
	int windowNamesSize = sizeof(windowNames) / sizeof(*windowNames);

	if (group) {
		for (int i = 0; i < windowNamesSize; i++) {
			windowNames[i] = groupWin;
		}
	}

	// create blur control window
	if (enable & EN_BLUR) {
		namedWindow(windowNames[0], CV_WINDOW_AUTOSIZE);
		createTrackbar("Blur", windowNames[0], &blurControl, 50);
	}

	// create contrast control window
	if (enable & EN_CONTRAST) {
		namedWindow(windowNames[1], CV_WINDOW_AUTOSIZE);
		createTrackbar("Beta", windowNames[1], &contrastBetaControl, 100.0f);
	}

	// create low & high color filter control windows
	if (enable & EN_COLOR) {
		// create low color filter control window
		namedWindow(windowNames[2], CV_WINDOW_AUTOSIZE);
		createTrackbar("Hue", windowNames[2], &lowHueControl, 255);
		createTrackbar("Saturation", windowNames[2], &lowSaturationControl,
				255);
		createTrackbar("Value", windowNames[2], &lowValueControl, 255);

		// create high color filter control window
		namedWindow(windowNames[3], CV_WINDOW_AUTOSIZE);
		createTrackbar("Hue", windowNames[3], &highHueControl, 255);
		createTrackbar("Saturation", windowNames[3], &highSaturationControl,
				255);
		createTrackbar("Value", windowNames[3], &highValueControl, 255);
	}

	// create threshold control window
	if (enable & EN_THRESH) {
		namedWindow(windowNames[4], CV_WINDOW_AUTOSIZE);
		createTrackbar("Threshold", windowNames[4], &thresholdControl, 255);
	}

	// create erode & dilate control window
	if (enable & EN_ED) {
		namedWindow(windowNames[5], CV_WINDOW_AUTOSIZE);
		createTrackbar("E & D", windowNames[5], &erodeDilateControl, 29);
	}

	// create canny edge detection control window
	if (enable & EN_CANNY) {
		namedWindow(windowNames[6], CV_WINDOW_AUTOSIZE);
		createTrackbar("Canny 1", windowNames[6], &cannyT1Control, 5000);
		createTrackbar("Canny 2", windowNames[6], &cannyT2Control, 5000);
	}

	// create approximate polygon control window
	if (enable & EN_POLY) {
		namedWindow(windowNames[7], CV_WINDOW_AUTOSIZE);
		createTrackbar("Approx Poly", windowNames[7], &approxPolyControl, 199);
	}
}

/**
 * Handles serial communication between the Arduinos.
 * Runs in a separate thread
 */
void *commThread(void *) {
#ifdef ENABLE_SERIAL
	int perPort1 = open_port("/dev/tty.usbserial-DA00SOQH");
	// add additional serials ports here ...
	sleep(1);// wait 1 second for arduino to boot
#endif
	pthread_mutex_lock(&dataMutex);
	for (int i = 0; i < 12; i++) {
		data[i] = 0;
	}
	pthread_mutex_unlock(&dataMutex);
	while (1) {
		float melodyZone[3];
		float rhythmZone[3];
		pthread_mutex_lock(&dataMutex);
		for (int i = 0; i < 6; i += 2) {
			if (data[i + 6] > 0) {
				rhythmZone[i / 2] = (float) data[i] / (float) data[i + 6];
			} else {
				rhythmZone[i / 2] = -1.0f;
			}
			data[i] = 0;
			data[i + 6] = 0;
			if (data[i + 7] > 0) {
				melodyZone[i / 2] = (float) data[i + 1] / (float) data[i + 7];
			} else {
				melodyZone[i / 2] = 9.0f;
			}
			data[i + 1] = 0;
			data[i + 7] = 0;
			melodyZone[i / 2] = round(melodyZone[i / 2]);
			rhythmZone[i / 2] = round(rhythmZone[i / 2]);
		}
		pthread_mutex_unlock(&dataMutex);
		if (rhythmZone[0] != -1) {
			char sendData[3] = { 0xAA, (char) melodyZone[0],
					(char) rhythmZone[0] };
#ifdef ENABLE_SERIAL
			write(perPort1, sendData, 3);
#endif
		}
		// these if's are for additional serial ports and corresponding instruments
		if (rhythmZone[1] != -1) {
//			fprintf(airPort1, "%c%c%c", 0xAA, (int) melodyZone[1], (int) rhythmZone[1]);
//			fprintf(airPort2, "%c%c%c", 0xAA, (int) melodyZone[1], (int) rhythmZone[1]);
			printf("Aerophone: %d, %d\n", (int) melodyZone[1],
					(int) rhythmZone[1]);
		}
		if (rhythmZone[2] != -1) {
//			fprintf(strPort1, "%c%c%c", 0xAA, (int) melodyZone[2], (int) rhythmZone[2]);
//			fprintf(strPort2, "%c%c%c", 0xAA, (int) melodyZone[2], (int) rhythmZone[2]);
			printf("Strings: %d, %d\n", (int) melodyZone[2],
					(int) rhythmZone[2]);
		}
		usleep(1500000);
		cout << "PRINTING FROM ANOTHER THREAD" << endl;
	}
#ifdef ENABLE_SERIAL
	close(perPort1);
#endif
	pthread_exit(NULL);
}
