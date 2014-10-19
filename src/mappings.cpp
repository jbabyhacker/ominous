/**
 * @authors Jason Klein
 * @authors Alexander O'Brien
 *
 * @copyright This project is released under the MIT License.
 */

#include "mappings.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>

using namespace cv;

/**
 * Maps a coordinate point to a predefined cube zone
 *
 * @param pos position of object
 * @param dim dimension of the video frame
 */
int pointToZoneRect(Point2f pos, Rect dim) {
	int xZone = pos.x / ((float) dim.width / 5.0f); // 0..4
	int yZone = pos.y / ((float) dim.height / 3.0f); // 0..2

	if (yZone == 1) {
		return xZone + 5;
	} else {
		return xZone;
	}
}

/**
 * Maps a coordinate point to a predefined sphere zone
 *
 * @param pos position of object
 * @param dim dimension of the video frame
 */
int pointToZoneCircle(Point2f pos, Rect dim) {
	int xZone = (float) pos.x / ((float) dim.width / 5.0f); // 0..4
	int yZone = (float) pos.y / ((float) dim.height / 3.0f); // 0..2

	if (xZone < 3) {
		return xZone * 3 + yZone;
	} else if (xZone == 3) {
		return xZone + yZone;
	} else {
		return yZone;
	}
}
