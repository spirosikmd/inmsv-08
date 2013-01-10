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
CND_PLATFORM=GNU-MacOSX
CND_DLIB_EXT=dylib
CND_CONF=Debug64bit
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/source/Colorbar.o \
	${OBJECTDIR}/source/Application.o \
	${OBJECTDIR}/source/Utilities.o \
	${OBJECTDIR}/source/Colormap.o \
	${OBJECTDIR}/source/Visualization.o \
	${OBJECTDIR}/source/main.o \
	${OBJECTDIR}/source/Simulation.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-m64 -ggdb -Wall -pipe
CXXFLAGS=-m64 -ggdb -Wall -pipe

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-Llibraries/fftw-2.1.5/ -lrfftw -lfftw

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/smoke

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/smoke: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -framework OpenGL -framework GLUT -framework GLUI -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/smoke ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/source/Colorbar.o: source/Colorbar.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -Ilibraries/fftw-2.1.5/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/Colorbar.o source/Colorbar.cpp

${OBJECTDIR}/source/Application.o: source/Application.cc 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -Ilibraries/fftw-2.1.5/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/Application.o source/Application.cc

${OBJECTDIR}/source/Utilities.o: source/Utilities.cc 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -Ilibraries/fftw-2.1.5/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/Utilities.o source/Utilities.cc

${OBJECTDIR}/source/Colormap.o: source/Colormap.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -Ilibraries/fftw-2.1.5/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/Colormap.o source/Colormap.cpp

${OBJECTDIR}/source/Visualization.o: source/Visualization.cpp 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -Ilibraries/fftw-2.1.5/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/Visualization.o source/Visualization.cpp

${OBJECTDIR}/source/main.o: source/main.cc 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -Ilibraries/fftw-2.1.5/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/main.o source/main.cc

${OBJECTDIR}/source/Simulation.o: source/Simulation.cc 
	${MKDIR} -p ${OBJECTDIR}/source
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -Ilibraries/fftw-2.1.5/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/source/Simulation.o source/Simulation.cc

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/smoke

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
