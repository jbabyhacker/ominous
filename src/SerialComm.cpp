/**
 * @authors Jason Klein
 * @authors Alexander O'Brien
 *
 * @copyright This project is released under the MIT License.
 */

#include "SerialComm.hpp"
#include <iostream>
#include <fcntl.h>
#include <termios.h>

using namespace std;

/**
 * @brief Opens the given serial port
 *
 * @param port The serial port to open (ex: "/dev/tty.usbserial-DA00SOQH")
 * @param baud The desired baud rate (ex: B9600)
 * @return A file descriptor that is used in conjunction with write(), read(),
 * and close() in unistd.h
 */
int openPort(const char *port, speed_t baud) {
	// open port and receive file descriptor
	int fd = open(port, O_WRONLY | O_NOCTTY | O_NDELAY);

	if (fd < 0) { // check for errors
		// port could not be opened
		cerr << "Unable to open port " << port << endl;
	} else {
		fcntl(fd, F_SETFL, 0);
	}

	struct termios options;

	// get current options for the port
	tcgetattr(fd, &options);

	// set baud rate
	cfsetispeed(&options, baud);
	cfsetospeed(&options, baud);

	// enable receiver and set local mode
	options.c_cflag |= (CLOCAL | CREAD);

	// set new options for the port
	tcsetattr(fd, TCSANOW, &options);

	return fd;
}
