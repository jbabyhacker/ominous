/**
 * @authors Jason Klein
 * @authors Alexander O'Brien
 *
 * @copyright This project is released under the MIT License.
 */


#ifndef MUSICTABLE_HPP_
#define MUSICTABLE_HPP_

#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

/**
 * @struct TrackedObject
 * @brief Information about correctly identified objects
 * @var TrackedObject::color
 * Color of the object where 'b'=blue, 'r'=red, and 'g'=green
 * @var TrackedObject::shape
 * Shape of object where 'r'=rectangle and 'c'=circle
 * @var TrackedObject::position
 * Coordinate location in the frame
 * @var TrackedObject::zone
 * Zone this object is located in
 */
typedef struct {
	char color;
	char shape;
	Point2f position;
	int zone;
} TrackedObject;

void initGuiControl(bool group, char enable);

#endif /* MUSICTABLE_HPP_ */
