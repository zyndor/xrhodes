#!/bin/bash

configurations="debug release"

pushd $(dirname $0)/../../.projects/linux/
for c in $configurations; do
echo Building $c
make all -j16 config=${c}_x64
done;
popd
