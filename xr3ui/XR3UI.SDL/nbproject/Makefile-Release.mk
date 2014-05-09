#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=MinGW-Windows
CND_DLIB_EXT=dll
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/1447016992/Screen.o \
	${OBJECTDIR}/_ext/1447016992/ScreenManager.o \
	${OBJECTDIR}/_ext/1447016992/UIAligner.o \
	${OBJECTDIR}/_ext/1447016992/UIBuilder.o \
	${OBJECTDIR}/_ext/1447016992/UIButton.o \
	${OBJECTDIR}/_ext/1447016992/UICascader.o \
	${OBJECTDIR}/_ext/1447016992/UICheckBox.o \
	${OBJECTDIR}/_ext/1447016992/UIColoredElement.o \
	${OBJECTDIR}/_ext/1447016992/UIContainer.o \
	${OBJECTDIR}/_ext/1447016992/UIElement.o \
	${OBJECTDIR}/_ext/1447016992/UIEventDispatcher.o \
	${OBJECTDIR}/_ext/1447016992/UIGridLayout.o \
	${OBJECTDIR}/_ext/1447016992/UIGrowingLayoutBase.o \
	${OBJECTDIR}/_ext/1447016992/UIHorizontalLayout.o \
	${OBJECTDIR}/_ext/1447016992/UIHorizontalProgressBar.o \
	${OBJECTDIR}/_ext/1447016992/UIHorizontalScrollingLayout.o \
	${OBJECTDIR}/_ext/1447016992/UIHorizontalSlider.o \
	${OBJECTDIR}/_ext/1447016992/UIImage.o \
	${OBJECTDIR}/_ext/1447016992/UIImagePanel.o \
	${OBJECTDIR}/_ext/1447016992/UILabel.o \
	${OBJECTDIR}/_ext/1447016992/UIProgressBarBase.o \
	${OBJECTDIR}/_ext/1447016992/UIRadioButton.o \
	${OBJECTDIR}/_ext/1447016992/UIRenderer.o \
	${OBJECTDIR}/_ext/1447016992/UISliderBase.o \
	${OBJECTDIR}/_ext/1447016992/UISpacer.o \
	${OBJECTDIR}/_ext/1447016992/UIState.o \
	${OBJECTDIR}/_ext/1447016992/UIVerticalLayout.o \
	${OBJECTDIR}/_ext/1447016992/UIVerticalProgressBar.o \
	${OBJECTDIR}/_ext/1447016992/UIVerticalScrollingLayout.o \
	${OBJECTDIR}/_ext/1447016992/UIVerticalSlider.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libxr3ui.sdl.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libxr3ui.sdl.a: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libxr3ui.sdl.a
	${AR} -rv ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libxr3ui.sdl.a ${OBJECTFILES} 
	$(RANLIB) ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libxr3ui.sdl.a

${OBJECTDIR}/_ext/1447016992/Screen.o: /D/repos/xr3/xr3ui/h/XR/Screen.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1447016992
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -DXR_SDL -I/C/tinyxml/ -I../../xr3/h/ -I../h/XR/ -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1447016992/Screen.o /D/repos/xr3/xr3ui/h/XR/Screen.cpp

${OBJECTDIR}/_ext/1447016992/ScreenManager.o: /D/repos/xr3/xr3ui/h/XR/ScreenManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1447016992
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -DXR_SDL -I/C/tinyxml/ -I../../xr3/h/ -I../h/XR/ -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1447016992/ScreenManager.o /D/repos/xr3/xr3ui/h/XR/ScreenManager.cpp

${OBJECTDIR}/_ext/1447016992/UIAligner.o: /D/repos/xr3/xr3ui/h/XR/UIAligner.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1447016992
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -DXR_SDL -I/C/tinyxml/ -I../../xr3/h/ -I../h/XR/ -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1447016992/UIAligner.o /D/repos/xr3/xr3ui/h/XR/UIAligner.cpp

${OBJECTDIR}/_ext/1447016992/UIBuilder.o: /D/repos/xr3/xr3ui/h/XR/UIBuilder.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1447016992
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -DXR_SDL -I/C/tinyxml/ -I../../xr3/h/ -I../h/XR/ -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1447016992/UIBuilder.o /D/repos/xr3/xr3ui/h/XR/UIBuilder.cpp

${OBJECTDIR}/_ext/1447016992/UIButton.o: /D/repos/xr3/xr3ui/h/XR/UIButton.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1447016992
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -DXR_SDL -I/C/tinyxml/ -I../../xr3/h/ -I../h/XR/ -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1447016992/UIButton.o /D/repos/xr3/xr3ui/h/XR/UIButton.cpp

${OBJECTDIR}/_ext/1447016992/UICascader.o: /D/repos/xr3/xr3ui/h/XR/UICascader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1447016992
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -DXR_SDL -I/C/tinyxml/ -I../../xr3/h/ -I../h/XR/ -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1447016992/UICascader.o /D/repos/xr3/xr3ui/h/XR/UICascader.cpp

${OBJECTDIR}/_ext/1447016992/UICheckBox.o: /D/repos/xr3/xr3ui/h/XR/UICheckBox.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1447016992
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -DXR_SDL -I/C/tinyxml/ -I../../xr3/h/ -I../h/XR/ -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1447016992/UICheckBox.o /D/repos/xr3/xr3ui/h/XR/UICheckBox.cpp

${OBJECTDIR}/_ext/1447016992/UIColoredElement.o: /D/repos/xr3/xr3ui/h/XR/UIColoredElement.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1447016992
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -DXR_SDL -I/C/tinyxml/ -I../../xr3/h/ -I../h/XR/ -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1447016992/UIColoredElement.o /D/repos/xr3/xr3ui/h/XR/UIColoredElement.cpp

${OBJECTDIR}/_ext/1447016992/UIContainer.o: /D/repos/xr3/xr3ui/h/XR/UIContainer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1447016992
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -DXR_SDL -I/C/tinyxml/ -I../../xr3/h/ -I../h/XR/ -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1447016992/UIContainer.o /D/repos/xr3/xr3ui/h/XR/UIContainer.cpp

${OBJECTDIR}/_ext/1447016992/UIElement.o: /D/repos/xr3/xr3ui/h/XR/UIElement.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1447016992
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -DXR_SDL -I/C/tinyxml/ -I../../xr3/h/ -I../h/XR/ -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1447016992/UIElement.o /D/repos/xr3/xr3ui/h/XR/UIElement.cpp

${OBJECTDIR}/_ext/1447016992/UIEventDispatcher.o: /D/repos/xr3/xr3ui/h/XR/UIEventDispatcher.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1447016992
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -DXR_SDL -I/C/tinyxml/ -I../../xr3/h/ -I../h/XR/ -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1447016992/UIEventDispatcher.o /D/repos/xr3/xr3ui/h/XR/UIEventDispatcher.cpp

${OBJECTDIR}/_ext/1447016992/UIGridLayout.o: /D/repos/xr3/xr3ui/h/XR/UIGridLayout.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1447016992
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -DXR_SDL -I/C/tinyxml/ -I../../xr3/h/ -I../h/XR/ -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1447016992/UIGridLayout.o /D/repos/xr3/xr3ui/h/XR/UIGridLayout.cpp

${OBJECTDIR}/_ext/1447016992/UIGrowingLayoutBase.o: /D/repos/xr3/xr3ui/h/XR/UIGrowingLayoutBase.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1447016992
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -DXR_SDL -I/C/tinyxml/ -I../../xr3/h/ -I../h/XR/ -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1447016992/UIGrowingLayoutBase.o /D/repos/xr3/xr3ui/h/XR/UIGrowingLayoutBase.cpp

${OBJECTDIR}/_ext/1447016992/UIHorizontalLayout.o: /D/repos/xr3/xr3ui/h/XR/UIHorizontalLayout.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1447016992
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -DXR_SDL -I/C/tinyxml/ -I../../xr3/h/ -I../h/XR/ -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1447016992/UIHorizontalLayout.o /D/repos/xr3/xr3ui/h/XR/UIHorizontalLayout.cpp

${OBJECTDIR}/_ext/1447016992/UIHorizontalProgressBar.o: /D/repos/xr3/xr3ui/h/XR/UIHorizontalProgressBar.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1447016992
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -DXR_SDL -I/C/tinyxml/ -I../../xr3/h/ -I../h/XR/ -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1447016992/UIHorizontalProgressBar.o /D/repos/xr3/xr3ui/h/XR/UIHorizontalProgressBar.cpp

${OBJECTDIR}/_ext/1447016992/UIHorizontalScrollingLayout.o: /D/repos/xr3/xr3ui/h/XR/UIHorizontalScrollingLayout.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1447016992
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -DXR_SDL -I/C/tinyxml/ -I../../xr3/h/ -I../h/XR/ -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1447016992/UIHorizontalScrollingLayout.o /D/repos/xr3/xr3ui/h/XR/UIHorizontalScrollingLayout.cpp

${OBJECTDIR}/_ext/1447016992/UIHorizontalSlider.o: /D/repos/xr3/xr3ui/h/XR/UIHorizontalSlider.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1447016992
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -DXR_SDL -I/C/tinyxml/ -I../../xr3/h/ -I../h/XR/ -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1447016992/UIHorizontalSlider.o /D/repos/xr3/xr3ui/h/XR/UIHorizontalSlider.cpp

${OBJECTDIR}/_ext/1447016992/UIImage.o: /D/repos/xr3/xr3ui/h/XR/UIImage.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1447016992
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -DXR_SDL -I/C/tinyxml/ -I../../xr3/h/ -I../h/XR/ -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1447016992/UIImage.o /D/repos/xr3/xr3ui/h/XR/UIImage.cpp

${OBJECTDIR}/_ext/1447016992/UIImagePanel.o: /D/repos/xr3/xr3ui/h/XR/UIImagePanel.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1447016992
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -DXR_SDL -I/C/tinyxml/ -I../../xr3/h/ -I../h/XR/ -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1447016992/UIImagePanel.o /D/repos/xr3/xr3ui/h/XR/UIImagePanel.cpp

${OBJECTDIR}/_ext/1447016992/UILabel.o: /D/repos/xr3/xr3ui/h/XR/UILabel.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1447016992
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -DXR_SDL -I/C/tinyxml/ -I../../xr3/h/ -I../h/XR/ -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1447016992/UILabel.o /D/repos/xr3/xr3ui/h/XR/UILabel.cpp

${OBJECTDIR}/_ext/1447016992/UIProgressBarBase.o: /D/repos/xr3/xr3ui/h/XR/UIProgressBarBase.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1447016992
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -DXR_SDL -I/C/tinyxml/ -I../../xr3/h/ -I../h/XR/ -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1447016992/UIProgressBarBase.o /D/repos/xr3/xr3ui/h/XR/UIProgressBarBase.cpp

${OBJECTDIR}/_ext/1447016992/UIRadioButton.o: /D/repos/xr3/xr3ui/h/XR/UIRadioButton.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1447016992
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -DXR_SDL -I/C/tinyxml/ -I../../xr3/h/ -I../h/XR/ -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1447016992/UIRadioButton.o /D/repos/xr3/xr3ui/h/XR/UIRadioButton.cpp

${OBJECTDIR}/_ext/1447016992/UIRenderer.o: /D/repos/xr3/xr3ui/h/XR/UIRenderer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1447016992
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -DXR_SDL -I/C/tinyxml/ -I../../xr3/h/ -I../h/XR/ -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1447016992/UIRenderer.o /D/repos/xr3/xr3ui/h/XR/UIRenderer.cpp

${OBJECTDIR}/_ext/1447016992/UISliderBase.o: /D/repos/xr3/xr3ui/h/XR/UISliderBase.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1447016992
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -DXR_SDL -I/C/tinyxml/ -I../../xr3/h/ -I../h/XR/ -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1447016992/UISliderBase.o /D/repos/xr3/xr3ui/h/XR/UISliderBase.cpp

${OBJECTDIR}/_ext/1447016992/UISpacer.o: /D/repos/xr3/xr3ui/h/XR/UISpacer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1447016992
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -DXR_SDL -I/C/tinyxml/ -I../../xr3/h/ -I../h/XR/ -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1447016992/UISpacer.o /D/repos/xr3/xr3ui/h/XR/UISpacer.cpp

${OBJECTDIR}/_ext/1447016992/UIState.o: /D/repos/xr3/xr3ui/h/XR/UIState.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1447016992
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -DXR_SDL -I/C/tinyxml/ -I../../xr3/h/ -I../h/XR/ -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1447016992/UIState.o /D/repos/xr3/xr3ui/h/XR/UIState.cpp

${OBJECTDIR}/_ext/1447016992/UIVerticalLayout.o: /D/repos/xr3/xr3ui/h/XR/UIVerticalLayout.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1447016992
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -DXR_SDL -I/C/tinyxml/ -I../../xr3/h/ -I../h/XR/ -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1447016992/UIVerticalLayout.o /D/repos/xr3/xr3ui/h/XR/UIVerticalLayout.cpp

${OBJECTDIR}/_ext/1447016992/UIVerticalProgressBar.o: /D/repos/xr3/xr3ui/h/XR/UIVerticalProgressBar.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1447016992
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -DXR_SDL -I/C/tinyxml/ -I../../xr3/h/ -I../h/XR/ -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1447016992/UIVerticalProgressBar.o /D/repos/xr3/xr3ui/h/XR/UIVerticalProgressBar.cpp

${OBJECTDIR}/_ext/1447016992/UIVerticalScrollingLayout.o: /D/repos/xr3/xr3ui/h/XR/UIVerticalScrollingLayout.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1447016992
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -DXR_SDL -I/C/tinyxml/ -I../../xr3/h/ -I../h/XR/ -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1447016992/UIVerticalScrollingLayout.o /D/repos/xr3/xr3ui/h/XR/UIVerticalScrollingLayout.cpp

${OBJECTDIR}/_ext/1447016992/UIVerticalSlider.o: /D/repos/xr3/xr3ui/h/XR/UIVerticalSlider.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1447016992
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -DXR_SDL -I/C/tinyxml/ -I../../xr3/h/ -I../h/XR/ -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1447016992/UIVerticalSlider.o /D/repos/xr3/xr3ui/h/XR/UIVerticalSlider.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libxr3ui.sdl.a

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
