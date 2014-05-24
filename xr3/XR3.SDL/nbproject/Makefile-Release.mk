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
	${OBJECTDIR}/_ext/241328751/glew.o \
	${OBJECTDIR}/_ext/1255643999/tinystr.o \
	${OBJECTDIR}/_ext/1255643999/tinyxml.o \
	${OBJECTDIR}/_ext/1255643999/tinyxmlerror.o \
	${OBJECTDIR}/_ext/1255643999/tinyxmlparser.o \
	${OBJECTDIR}/_ext/1046665460/AABB.o \
	${OBJECTDIR}/_ext/1046665460/ActorInstance.o \
	${OBJECTDIR}/_ext/1046665460/ActorReader.o \
	${OBJECTDIR}/_ext/1046665460/Animation.o \
	${OBJECTDIR}/_ext/1046665460/AnimationInstance.o \
	${OBJECTDIR}/_ext/1046665460/AnimationPlayer.o \
	${OBJECTDIR}/_ext/1046665460/AnimationReader.o \
	${OBJECTDIR}/_ext/1046665460/Application.o \
	${OBJECTDIR}/_ext/1046665460/BasicMesh.o \
	${OBJECTDIR}/_ext/1046665460/Callback.o \
	${OBJECTDIR}/_ext/1046665460/Color.o \
	${OBJECTDIR}/_ext/1046665460/CrossObject.o \
	${OBJECTDIR}/_ext/1046665460/DelayedCallback.o \
	${OBJECTDIR}/_ext/1046665460/Entity.o \
	${OBJECTDIR}/_ext/1046665460/EventDispatcher.o \
	${OBJECTDIR}/_ext/1046665460/Font.o \
	${OBJECTDIR}/_ext/1046665460/Grid.o \
	${OBJECTDIR}/_ext/1046665460/Hash.o \
	${OBJECTDIR}/_ext/1046665460/IndexMesh.o \
	${OBJECTDIR}/_ext/1046665460/Indexer2d.o \
	${OBJECTDIR}/_ext/1046665460/JsonReader.o \
	${OBJECTDIR}/_ext/1046665460/JsonWriter.o \
	${OBJECTDIR}/_ext/1046665460/Log.o \
	${OBJECTDIR}/_ext/1046665460/MersenneTwister.o \
	${OBJECTDIR}/_ext/1046665460/MeshRenderer.o \
	${OBJECTDIR}/_ext/1046665460/Module.o \
	${OBJECTDIR}/_ext/1046665460/Named.o \
	${OBJECTDIR}/_ext/1046665460/Parse.o \
	${OBJECTDIR}/_ext/1046665460/ParserCore.o \
	${OBJECTDIR}/_ext/1046665460/Pool.o \
	${OBJECTDIR}/_ext/1046665460/Quadtree.o \
	${OBJECTDIR}/_ext/1046665460/Ray.o \
	${OBJECTDIR}/_ext/1046665460/RectObject.o \
	${OBJECTDIR}/_ext/1046665460/RingBuffer.o \
	${OBJECTDIR}/_ext/784063616/Audio.o \
	${OBJECTDIR}/_ext/784063616/Device.o \
	${OBJECTDIR}/_ext/784063616/File.o \
	${OBJECTDIR}/_ext/784063616/Input.o \
	${OBJECTDIR}/_ext/784063616/InputImpl.o \
	${OBJECTDIR}/_ext/784063616/KeyCode.o \
	${OBJECTDIR}/_ext/784063616/Material.o \
	${OBJECTDIR}/_ext/784063616/MaterialImpl.o \
	${OBJECTDIR}/_ext/784063616/MouseButton.o \
	${OBJECTDIR}/_ext/784063616/RenderStream.o \
	${OBJECTDIR}/_ext/784063616/Renderer.o \
	${OBJECTDIR}/_ext/784063616/Sample.o \
	${OBJECTDIR}/_ext/784063616/Texture.o \
	${OBJECTDIR}/_ext/784063616/TextureImpl.o \
	${OBJECTDIR}/_ext/784063616/Timer.o \
	${OBJECTDIR}/_ext/784063616/Worker.o \
	${OBJECTDIR}/_ext/1046665460/Serialized.o \
	${OBJECTDIR}/_ext/1046665460/SharedPoolAllocator.o \
	${OBJECTDIR}/_ext/1046665460/Sprite.o \
	${OBJECTDIR}/_ext/1046665460/SpriteRenderer.o \
	${OBJECTDIR}/_ext/1046665460/State.o \
	${OBJECTDIR}/_ext/1046665460/Text.o \
	${OBJECTDIR}/_ext/1046665460/TexturePack.o \
	${OBJECTDIR}/_ext/1046665460/TweenVar.o \
	${OBJECTDIR}/_ext/1046665460/TweenVarPool.o \
	${OBJECTDIR}/_ext/1046665460/Tweener.o \
	${OBJECTDIR}/_ext/1046665460/fundamentals.o \
	${OBJECTDIR}/_ext/1046665460/json.o \
	${OBJECTDIR}/_ext/1046665460/maths.o \
	${OBJECTDIR}/_ext/1046665460/utils.o \
	${OBJECTDIR}/_ext/1046665460/vectypes.o


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
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libxr3.sdl.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libxr3.sdl.a: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libxr3.sdl.a
	${AR} -rv ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libxr3.sdl.a ${OBJECTFILES} 
	$(RANLIB) ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libxr3.sdl.a

${OBJECTDIR}/_ext/241328751/glew.o: /C/glew-1.10.0/src/glew.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/241328751
	${RM} "$@.d"
	$(COMPILE.c) -O2 -I/D/repos/xr3/xr3/h -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/241328751/glew.o /C/glew-1.10.0/src/glew.c

${OBJECTDIR}/_ext/1255643999/tinystr.o: /C/tinyxml/tinystr.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1255643999
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1255643999/tinystr.o /C/tinyxml/tinystr.cpp

${OBJECTDIR}/_ext/1255643999/tinyxml.o: /C/tinyxml/tinyxml.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1255643999
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1255643999/tinyxml.o /C/tinyxml/tinyxml.cpp

${OBJECTDIR}/_ext/1255643999/tinyxmlerror.o: /C/tinyxml/tinyxmlerror.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1255643999
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1255643999/tinyxmlerror.o /C/tinyxml/tinyxmlerror.cpp

${OBJECTDIR}/_ext/1255643999/tinyxmlparser.o: /C/tinyxml/tinyxmlparser.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1255643999
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1255643999/tinyxmlparser.o /C/tinyxml/tinyxmlparser.cpp

${OBJECTDIR}/_ext/1046665460/AABB.o: /D/repos/xr3/xr3/h/XR/AABB.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1046665460
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1046665460/AABB.o /D/repos/xr3/xr3/h/XR/AABB.cpp

${OBJECTDIR}/_ext/1046665460/ActorInstance.o: /D/repos/xr3/xr3/h/XR/ActorInstance.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1046665460
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1046665460/ActorInstance.o /D/repos/xr3/xr3/h/XR/ActorInstance.cpp

${OBJECTDIR}/_ext/1046665460/ActorReader.o: /D/repos/xr3/xr3/h/XR/ActorReader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1046665460
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1046665460/ActorReader.o /D/repos/xr3/xr3/h/XR/ActorReader.cpp

${OBJECTDIR}/_ext/1046665460/Animation.o: /D/repos/xr3/xr3/h/XR/Animation.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1046665460
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1046665460/Animation.o /D/repos/xr3/xr3/h/XR/Animation.cpp

${OBJECTDIR}/_ext/1046665460/AnimationInstance.o: /D/repos/xr3/xr3/h/XR/AnimationInstance.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1046665460
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1046665460/AnimationInstance.o /D/repos/xr3/xr3/h/XR/AnimationInstance.cpp

${OBJECTDIR}/_ext/1046665460/AnimationPlayer.o: /D/repos/xr3/xr3/h/XR/AnimationPlayer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1046665460
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1046665460/AnimationPlayer.o /D/repos/xr3/xr3/h/XR/AnimationPlayer.cpp

${OBJECTDIR}/_ext/1046665460/AnimationReader.o: /D/repos/xr3/xr3/h/XR/AnimationReader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1046665460
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1046665460/AnimationReader.o /D/repos/xr3/xr3/h/XR/AnimationReader.cpp

${OBJECTDIR}/_ext/1046665460/Application.o: /D/repos/xr3/xr3/h/XR/Application.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1046665460
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1046665460/Application.o /D/repos/xr3/xr3/h/XR/Application.cpp

${OBJECTDIR}/_ext/1046665460/BasicMesh.o: /D/repos/xr3/xr3/h/XR/BasicMesh.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1046665460
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1046665460/BasicMesh.o /D/repos/xr3/xr3/h/XR/BasicMesh.cpp

${OBJECTDIR}/_ext/1046665460/Callback.o: /D/repos/xr3/xr3/h/XR/Callback.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1046665460
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1046665460/Callback.o /D/repos/xr3/xr3/h/XR/Callback.cpp

${OBJECTDIR}/_ext/1046665460/Color.o: /D/repos/xr3/xr3/h/XR/Color.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1046665460
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1046665460/Color.o /D/repos/xr3/xr3/h/XR/Color.cpp

${OBJECTDIR}/_ext/1046665460/CrossObject.o: /D/repos/xr3/xr3/h/XR/CrossObject.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1046665460
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1046665460/CrossObject.o /D/repos/xr3/xr3/h/XR/CrossObject.cpp

${OBJECTDIR}/_ext/1046665460/DelayedCallback.o: /D/repos/xr3/xr3/h/XR/DelayedCallback.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1046665460
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1046665460/DelayedCallback.o /D/repos/xr3/xr3/h/XR/DelayedCallback.cpp

${OBJECTDIR}/_ext/1046665460/Entity.o: /D/repos/xr3/xr3/h/XR/Entity.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1046665460
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1046665460/Entity.o /D/repos/xr3/xr3/h/XR/Entity.cpp

${OBJECTDIR}/_ext/1046665460/EventDispatcher.o: /D/repos/xr3/xr3/h/XR/EventDispatcher.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1046665460
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1046665460/EventDispatcher.o /D/repos/xr3/xr3/h/XR/EventDispatcher.cpp

${OBJECTDIR}/_ext/1046665460/Font.o: /D/repos/xr3/xr3/h/XR/Font.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1046665460
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1046665460/Font.o /D/repos/xr3/xr3/h/XR/Font.cpp

${OBJECTDIR}/_ext/1046665460/Grid.o: /D/repos/xr3/xr3/h/XR/Grid.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1046665460
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1046665460/Grid.o /D/repos/xr3/xr3/h/XR/Grid.cpp

${OBJECTDIR}/_ext/1046665460/Hash.o: /D/repos/xr3/xr3/h/XR/Hash.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1046665460
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1046665460/Hash.o /D/repos/xr3/xr3/h/XR/Hash.cpp

${OBJECTDIR}/_ext/1046665460/IndexMesh.o: /D/repos/xr3/xr3/h/XR/IndexMesh.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1046665460
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1046665460/IndexMesh.o /D/repos/xr3/xr3/h/XR/IndexMesh.cpp

${OBJECTDIR}/_ext/1046665460/Indexer2d.o: /D/repos/xr3/xr3/h/XR/Indexer2d.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1046665460
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1046665460/Indexer2d.o /D/repos/xr3/xr3/h/XR/Indexer2d.cpp

${OBJECTDIR}/_ext/1046665460/JsonReader.o: /D/repos/xr3/xr3/h/XR/JsonReader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1046665460
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1046665460/JsonReader.o /D/repos/xr3/xr3/h/XR/JsonReader.cpp

${OBJECTDIR}/_ext/1046665460/JsonWriter.o: /D/repos/xr3/xr3/h/XR/JsonWriter.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1046665460
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1046665460/JsonWriter.o /D/repos/xr3/xr3/h/XR/JsonWriter.cpp

${OBJECTDIR}/_ext/1046665460/Log.o: /D/repos/xr3/xr3/h/XR/Log.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1046665460
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1046665460/Log.o /D/repos/xr3/xr3/h/XR/Log.cpp

${OBJECTDIR}/_ext/1046665460/MersenneTwister.o: /D/repos/xr3/xr3/h/XR/MersenneTwister.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1046665460
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1046665460/MersenneTwister.o /D/repos/xr3/xr3/h/XR/MersenneTwister.cpp

${OBJECTDIR}/_ext/1046665460/MeshRenderer.o: /D/repos/xr3/xr3/h/XR/MeshRenderer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1046665460
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1046665460/MeshRenderer.o /D/repos/xr3/xr3/h/XR/MeshRenderer.cpp

${OBJECTDIR}/_ext/1046665460/Module.o: /D/repos/xr3/xr3/h/XR/Module.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1046665460
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1046665460/Module.o /D/repos/xr3/xr3/h/XR/Module.cpp

${OBJECTDIR}/_ext/1046665460/Named.o: /D/repos/xr3/xr3/h/XR/Named.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1046665460
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1046665460/Named.o /D/repos/xr3/xr3/h/XR/Named.cpp

${OBJECTDIR}/_ext/1046665460/Parse.o: /D/repos/xr3/xr3/h/XR/Parse.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1046665460
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1046665460/Parse.o /D/repos/xr3/xr3/h/XR/Parse.cpp

${OBJECTDIR}/_ext/1046665460/ParserCore.o: /D/repos/xr3/xr3/h/XR/ParserCore.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1046665460
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1046665460/ParserCore.o /D/repos/xr3/xr3/h/XR/ParserCore.cpp

${OBJECTDIR}/_ext/1046665460/Pool.o: /D/repos/xr3/xr3/h/XR/Pool.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1046665460
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1046665460/Pool.o /D/repos/xr3/xr3/h/XR/Pool.cpp

${OBJECTDIR}/_ext/1046665460/Quadtree.o: /D/repos/xr3/xr3/h/XR/Quadtree.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1046665460
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1046665460/Quadtree.o /D/repos/xr3/xr3/h/XR/Quadtree.cpp

${OBJECTDIR}/_ext/1046665460/Ray.o: /D/repos/xr3/xr3/h/XR/Ray.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1046665460
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1046665460/Ray.o /D/repos/xr3/xr3/h/XR/Ray.cpp

${OBJECTDIR}/_ext/1046665460/RectObject.o: /D/repos/xr3/xr3/h/XR/RectObject.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1046665460
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1046665460/RectObject.o /D/repos/xr3/xr3/h/XR/RectObject.cpp

${OBJECTDIR}/_ext/1046665460/RingBuffer.o: /D/repos/xr3/xr3/h/XR/RingBuffer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1046665460
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1046665460/RingBuffer.o /D/repos/xr3/xr3/h/XR/RingBuffer.cpp

${OBJECTDIR}/_ext/784063616/Audio.o: /D/repos/xr3/xr3/h/XR/SDL/Audio.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/784063616
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/784063616/Audio.o /D/repos/xr3/xr3/h/XR/SDL/Audio.cpp

${OBJECTDIR}/_ext/784063616/Device.o: /D/repos/xr3/xr3/h/XR/SDL/Device.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/784063616
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/784063616/Device.o /D/repos/xr3/xr3/h/XR/SDL/Device.cpp

${OBJECTDIR}/_ext/784063616/File.o: /D/repos/xr3/xr3/h/XR/SDL/File.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/784063616
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/784063616/File.o /D/repos/xr3/xr3/h/XR/SDL/File.cpp

${OBJECTDIR}/_ext/784063616/Input.o: /D/repos/xr3/xr3/h/XR/SDL/Input.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/784063616
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/784063616/Input.o /D/repos/xr3/xr3/h/XR/SDL/Input.cpp

${OBJECTDIR}/_ext/784063616/InputImpl.o: /D/repos/xr3/xr3/h/XR/SDL/InputImpl.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/784063616
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/784063616/InputImpl.o /D/repos/xr3/xr3/h/XR/SDL/InputImpl.cpp

${OBJECTDIR}/_ext/784063616/KeyCode.o: /D/repos/xr3/xr3/h/XR/SDL/KeyCode.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/784063616
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/784063616/KeyCode.o /D/repos/xr3/xr3/h/XR/SDL/KeyCode.cpp

${OBJECTDIR}/_ext/784063616/Material.o: /D/repos/xr3/xr3/h/XR/SDL/Material.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/784063616
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/784063616/Material.o /D/repos/xr3/xr3/h/XR/SDL/Material.cpp

${OBJECTDIR}/_ext/784063616/MaterialImpl.o: /D/repos/xr3/xr3/h/XR/SDL/MaterialImpl.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/784063616
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/784063616/MaterialImpl.o /D/repos/xr3/xr3/h/XR/SDL/MaterialImpl.cpp

${OBJECTDIR}/_ext/784063616/MouseButton.o: /D/repos/xr3/xr3/h/XR/SDL/MouseButton.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/784063616
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/784063616/MouseButton.o /D/repos/xr3/xr3/h/XR/SDL/MouseButton.cpp

${OBJECTDIR}/_ext/784063616/RenderStream.o: /D/repos/xr3/xr3/h/XR/SDL/RenderStream.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/784063616
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/784063616/RenderStream.o /D/repos/xr3/xr3/h/XR/SDL/RenderStream.cpp

${OBJECTDIR}/_ext/784063616/Renderer.o: /D/repos/xr3/xr3/h/XR/SDL/Renderer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/784063616
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/784063616/Renderer.o /D/repos/xr3/xr3/h/XR/SDL/Renderer.cpp

${OBJECTDIR}/_ext/784063616/Sample.o: /D/repos/xr3/xr3/h/XR/SDL/Sample.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/784063616
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/784063616/Sample.o /D/repos/xr3/xr3/h/XR/SDL/Sample.cpp

${OBJECTDIR}/_ext/784063616/Texture.o: /D/repos/xr3/xr3/h/XR/SDL/Texture.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/784063616
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/784063616/Texture.o /D/repos/xr3/xr3/h/XR/SDL/Texture.cpp

${OBJECTDIR}/_ext/784063616/TextureImpl.o: /D/repos/xr3/xr3/h/XR/SDL/TextureImpl.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/784063616
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/784063616/TextureImpl.o /D/repos/xr3/xr3/h/XR/SDL/TextureImpl.cpp

${OBJECTDIR}/_ext/784063616/Timer.o: /D/repos/xr3/xr3/h/XR/SDL/Timer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/784063616
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/784063616/Timer.o /D/repos/xr3/xr3/h/XR/SDL/Timer.cpp

${OBJECTDIR}/_ext/784063616/Worker.o: /D/repos/xr3/xr3/h/XR/SDL/Worker.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/784063616
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/784063616/Worker.o /D/repos/xr3/xr3/h/XR/SDL/Worker.cpp

${OBJECTDIR}/_ext/1046665460/Serialized.o: /D/repos/xr3/xr3/h/XR/Serialized.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1046665460
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1046665460/Serialized.o /D/repos/xr3/xr3/h/XR/Serialized.cpp

${OBJECTDIR}/_ext/1046665460/SharedPoolAllocator.o: /D/repos/xr3/xr3/h/XR/SharedPoolAllocator.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1046665460
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1046665460/SharedPoolAllocator.o /D/repos/xr3/xr3/h/XR/SharedPoolAllocator.cpp

${OBJECTDIR}/_ext/1046665460/Sprite.o: /D/repos/xr3/xr3/h/XR/Sprite.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1046665460
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1046665460/Sprite.o /D/repos/xr3/xr3/h/XR/Sprite.cpp

${OBJECTDIR}/_ext/1046665460/SpriteRenderer.o: /D/repos/xr3/xr3/h/XR/SpriteRenderer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1046665460
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1046665460/SpriteRenderer.o /D/repos/xr3/xr3/h/XR/SpriteRenderer.cpp

${OBJECTDIR}/_ext/1046665460/State.o: /D/repos/xr3/xr3/h/XR/State.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1046665460
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1046665460/State.o /D/repos/xr3/xr3/h/XR/State.cpp

${OBJECTDIR}/_ext/1046665460/Text.o: /D/repos/xr3/xr3/h/XR/Text.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1046665460
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1046665460/Text.o /D/repos/xr3/xr3/h/XR/Text.cpp

${OBJECTDIR}/_ext/1046665460/TexturePack.o: /D/repos/xr3/xr3/h/XR/TexturePack.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1046665460
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1046665460/TexturePack.o /D/repos/xr3/xr3/h/XR/TexturePack.cpp

${OBJECTDIR}/_ext/1046665460/TweenVar.o: /D/repos/xr3/xr3/h/XR/TweenVar.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1046665460
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1046665460/TweenVar.o /D/repos/xr3/xr3/h/XR/TweenVar.cpp

${OBJECTDIR}/_ext/1046665460/TweenVarPool.o: /D/repos/xr3/xr3/h/XR/TweenVarPool.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1046665460
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1046665460/TweenVarPool.o /D/repos/xr3/xr3/h/XR/TweenVarPool.cpp

${OBJECTDIR}/_ext/1046665460/Tweener.o: /D/repos/xr3/xr3/h/XR/Tweener.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1046665460
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1046665460/Tweener.o /D/repos/xr3/xr3/h/XR/Tweener.cpp

${OBJECTDIR}/_ext/1046665460/fundamentals.o: /D/repos/xr3/xr3/h/XR/fundamentals.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1046665460
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1046665460/fundamentals.o /D/repos/xr3/xr3/h/XR/fundamentals.cpp

${OBJECTDIR}/_ext/1046665460/json.o: /D/repos/xr3/xr3/h/XR/json.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1046665460
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1046665460/json.o /D/repos/xr3/xr3/h/XR/json.cpp

${OBJECTDIR}/_ext/1046665460/maths.o: /D/repos/xr3/xr3/h/XR/maths.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1046665460
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1046665460/maths.o /D/repos/xr3/xr3/h/XR/maths.cpp

${OBJECTDIR}/_ext/1046665460/utils.o: /D/repos/xr3/xr3/h/XR/utils.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1046665460
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1046665460/utils.o /D/repos/xr3/xr3/h/XR/utils.cpp

${OBJECTDIR}/_ext/1046665460/vectypes.o: /D/repos/xr3/xr3/h/XR/vectypes.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1046665460
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DGLEW_STATIC -DXR_SDL -I/C/tinyxml -I/C/glew-1.10.0/src -I../h/XR -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1046665460/vectypes.o /D/repos/xr3/xr3/h/XR/vectypes.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libxr3.sdl.a

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
