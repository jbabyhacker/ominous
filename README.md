#About

Final project for MU201X, Making Music With Machines.

#### OS Support

  Ominous has been tested on Mac OS X but it should also work on linux.
  Windows support is unknown because the serial communication code works on POSIX systems only.

#### Dependencies

  On Mac OS X, I recommend using homebrew to install autoconf and OpenCV. It can be downloaded from http://brew.sh.
  Next install the homebrew tap, autoconf, and OpenCV:
  ```
  $ brew tap homebrew/science
  $ brew install automake autoconf doxygen opencv
  ```
#### Building

  ```
  $ git clone https://github.com/jbabyhacker/ominous.git
  $ cd ominous
  $ sh autogen.sh && make
  ```

#### Documentation

  Doxygen documentation can be built by running:
  ```
  $ sh gendoc.sh
  ```
  
