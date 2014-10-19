#!/bin/sh

# welcome message
echo Restoring package to original state

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
echo Restore completed successfully