#!/bin/bash

git checkout master

# check first line of CHANGES for latest version; save it.
version=$(head -1 $(dirname $0)/../../CHANGES)

# check version against regex.
echo $version | grep -E "^v(0|[1-9][0-9]*)(\.(0|[1-9][0-9]*)){2}$" > /dev/null
if (( $? != 0 )); then
  echo Invalid version "$version".
  exit 1;
fi

# ensure it isn't a version that we have already released.
git tag | grep $version > /dev/null
if (( $? == 0 )); then
  echo Version "$version" was already released. Have you run update-changes.sh?
  exit 1;
fi

# create temporary file with random name to store commit message
msg_file=$(echo $RANDOM).$(echo $$).tmp

# write version to message
echo $version >> $msg_file
echo >> $msg_file

# write changes since last release, to message
last_changelog=$(git log --oneline --no-decorate --format=format:%H -1 "../../CHANGES")
git log --oneline --no-decorate --reverse RELEASE..$last_changelog | cut -c 9- | sed "s/\*/- /g" >> $msg_file

# hop over to RELEASE and create a commit with message from file
git checkout RELEASE
git merge --no-ff -F $msg_file master

# tag version
git tag $version

# clean up
rm $msg_file
