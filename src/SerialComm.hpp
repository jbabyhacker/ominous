/**
 * @authors Jason Klein
 * @authors Alexander O'Brien
 *
 * @copyright This project is released under the MIT License.
 */

#ifndef SERIALCOMM_HPP_
#define SERIALCOMM_HPP_

#include <termios.h>

int openPort(const char *port, speed_t baud);

#endif /* SERIALCOMM_HPP_ */
