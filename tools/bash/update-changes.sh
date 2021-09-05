#!/bin/bash

if [ -z $1 ]
then
  echo "Usage: ./update-changes.sh [version]"
  exit 1
fi

changes=$(dirname $0)/../../CHANGES

# paste version to new changes file
echo $1 > $changes.new

# past recent changes to new changes file
git log --oneline --reverse --no-decorate RELEASE.. | cut -c 9- | sed "s/^/- /g" >> $changes.new
echo >> $changes.new

# past old changes to new changes file
cat $changes >> $changes.new

# overwrite old changes file with new
rm $changes
mv $changes{.new,}

# stage & commit
git add $changes
git commit -m "Changelog update for $1."
