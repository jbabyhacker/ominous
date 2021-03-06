#!/bin/sh

#################################################################
# Jason Klein - restore.sh                                      #
# March 2015                                                    #
# Restores directory to initial state by removing files created #
# by autogen.sh, configure, make, and gendoc.sh                 #
#################################################################

# welcome message
echo "#######################################"
echo "# Restoring project to original state #"
echo "#######################################"

sleep 0.5

# set list of files and or directories to be deleted
purgeList=(autom4te.cache aclocal.m4 config.guess config.sub configure depcomp INSTALL install-sh Makefile.in missing src/Makefile.in doc);

# clean up executable, object files, and makefile
make distclean

# delete every item in purgeList
for t in ${purgeList[@]}
do
echo Deleting $t
rm -rf $t
done

# closing message
echo "#####################"
echo "# Restore completed #"
echo "#####################"