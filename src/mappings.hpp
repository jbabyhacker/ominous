/**
 * @authors Jason Klein
 * @authors Alexander O'Brien
 *
 * @copyright This project is released under the MIT License.
 */

#ifndef MAPPINGS_HPP_
#define MAPPINGS_HPP_

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>

using namespace cv;

int pointToZoneRect(Point2f pos, Rect dim);
int pointToZoneCircle(Point2f pos, Rect dim);

#endif /* MAPPINGS_HPP_ */
