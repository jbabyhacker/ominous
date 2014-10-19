#!/bin/sh

# welcome message
echo Regenerating autotools files

# run autoreconf to copy necessary executables into this directory
autoreconf --install --force || exit 1

# configure project
./configure