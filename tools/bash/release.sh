#!/bin/bash

git checkout master

# first line of CHANGES is latest version - save it.
version=$(head -1 $(dirname $0)/../../CHANGES)

# create temporary file with random name to store commit message
msg_file=$(echo $RANDOM).$(echo $$).tmp

# write version to message
echo $version >> $msg_file
echo >> $msg_file

# write changes since last release, to message
git qlog RELEASE..HEAD^ | cut -c -2,11- | sed "s/\*/-/g" >> $msg_file

# hop over to RELEASE and create a commit with message from file
git checkout RELEASE
git merge --no-ff -F $msg_file master

# tag version
git tag $version

# clean up
rm $msg_file
