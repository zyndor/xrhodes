#!/bin/sh
#
# XRhodes
#
# copyright (c) Gyorgy Straub. All rights reserved.
#
# License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
#
#===============================================================================
# Generates xcode4 projects for macOS.
./premake5 --target=macos xcode4 --file=../../premake5.lua