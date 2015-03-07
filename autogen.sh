#!/bin/sh

##################################################################
# Jason Klein - autogen.sh                                       #
# March 2015                                                     #
# Regenerates autotools files and then configures the project.   #
# If this file is executed with and agurment of, all, then the   #
# project will additionally be built and documentation generated #
##################################################################

# welcome message
echo "################################"
echo "# Regenerating autotools files #"
echo "################################"

sleep 0.5

# run autoreconf to copy necessary executables into this directory
autoreconf --install --force --warnings all || exit 1


# configure message
echo "#######################"
echo "# Configuring project #"
echo "#######################"

sleep 0.5

# configure project
./configure


if [ "$1" = "all" ]; then
	echo "####################"
	echo "# Building project #"
	echo "####################"
	
	sleep 0.5
	
	make
	
	sh gendoc.sh
	
	echo "###############################"
	echo "# Exection has been completed #"
	echo "###############################"
else
	echo "#################################################"
	echo "# Build the project & documentation by running: #"
	echo "#    make && sh gendoc.sh                       #"
	echo "#################################################"
fi



