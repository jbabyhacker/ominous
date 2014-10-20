#About

Final project for MU201X, Making Music With Machines. Arduino code can be found at https://github.com/jbabyhacker/omino

![screenshot](https://raw.github.com/jbabyhacker/ominous/master/ominous.jpg)

The idea behind our design is to control the instruments as the conductor. Since most artists compose for all of their machines by themself, we wanted to allow for more than one person to control the musical machines at the same time. It consists of a webcam mounted above a 3x2 feet black felt surface in which the operator moves colored cubes and spheres around it. Different colors are mapped to different musical machines. There are 6 blocks total, 3 sphere and 3 cubes, while each cube-sphere set is blue, green, and pink. Moving the cubes around the surface control the rhythm for that instrument while moving the sphere control the melody. 

The mappings for rhythm and melody are independent of each other, meaning, if a cube and sphere are at the same location, the selected index for rhythm and melody could be different. The table updates every measure (~1.5 seconds) by averaging position data and then sending two numbers, 0-9,to each musical machine. One number corresponds to a rhythm select while the other corresponds to the melody select. These rhythms and melodies are hard coded onto each musical machine. 

Future Improvements
- [ ] Make camera sensing software more robust (currently it is sensitive to lights and shadows)
- [ ] Use multiple threads to speed up sample rate
- [ ] Use flat paint on objects, glossy paint reflects the light
- [ ] Don't have hard coded rhythms and melodies on the Arduino

Possible features to add more control parameters
- [ ]	Instead of having objects mapped to a virtual grid, have it be in relations to other objects
- [ ]	Have a different color on each side of the cube
- [ ]	Take advantage of the rotation orientation of the cube
- [ ]	Add different objects (ex: triangles
- [ ]	Integrate potentiometers and or buttons into table


#### OS Support

  Ominous has been tested on Mac OS X but it should also work on linux. Serial communication code works on POSIX systems only.

#### Dependencies

  On Mac OS X, I recommend using homebrew to install autoconf and OpenCV. It can be downloaded from http://brew.sh.
  Next install the homebrew tap, automake, autoconf, doxygen, and OpenCV:
  ```
  $ brew tap homebrew/science
  $ brew install automake autoconf doxygen opencv
  ```
#### Building

  The autogen.sh script will run autoreconf and then configure the project:
  ```
  $ sh autogen.sh && make
  ```

#### Documentation

  Doxygen documentation can be built by running:
  ```
  $ sh gendoc.sh
  ```
  
#### Restore

  You can restore the directory to its pre-autoreconf and configure state:
  ```
  $ sh restore.sh
  ```
