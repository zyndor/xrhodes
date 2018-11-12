#!/bin/sh
#
# XRhodes
#
# copyright (c) Gyorgy Straub. All rights reserved.
#
# License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
#
#===============================================================================
# Generates makefile projects for linux.

SCRIPT_DIR=$(dirname $0)

$SCRIPT_DIR/premake5 --target=linux gmake --file=$SCRIPT_DIR/../../premake5.lua
