#!/bin/bash
#
# XRhodes
#
# copyright (c) Gyorgy Straub. All rights reserved.
#
# License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
#
#==============================================================================
# Installs the SDL2 libraries.

TARGET=/Library/Frameworks/

sudo cp -R ../../external/SDL2/SDL2.framework $TARGET
