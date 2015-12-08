CPP = g++
DEBUGFLAGS = -g -std=c++11
RELEASEFLAGS = -s -std=c++11 -O3 -Wall
BUILDFLAGS = $(DEBUGFLAGS)
SRCDIR = ./sources/
OBJDIR = ./debug/

SOURCES = Angle.cpp Axis.cpp BaseNPC.cpp BoundingBox.cpp	\
CompileTools.cpp Connection.cpp DispInfo.cpp Editor.cpp		\
Entity.cpp KeyValBase.cpp KeyVal.cpp main.cpp Matrix.cpp	\
Part.cpp Plane.cpp Quaternion.cpp Side.cpp SimplexNoise.cpp	\
Solid.cpp Vector.cpp Vertex.cpp World.cpp

OBJECTS = $(SOURCES:.cpp=.o)

HEADERS = Angle.h Axis.h BoundingBox.hConnection.h        \
Editor.h Entity.h KeyVal.h LinkedList.h Matrix.h          \
Part.h Plane.h Primes.h Quaternion.h Side.h Solid.h       \
stdafx.h Timer.h utils.h Vector.h Vertex.h WeightedList.h \
World.h

ifeq ($(MAKECMDGOALS),rndrelease)
BUILDFLAGS = $(RELEASEFLAGS)
OBJDIR = ./release/
else ifeq ($(MAKECMDGOALS),cleanrelease)
OBJDIR = ./release/
else
BUILDFLAGS = $(DEBUGFLAGS)
endif

BIN = $(OBJDIR)rndlevelsource
OBJS = $(addprefix $(OBJDIR),$(OBJECTS))

rndebug: | $(OBJDIR) $(BIN)

rndrelease: | $(OBJDIR) $(BIN)

$(BIN): $(OBJS)
	$(CPP) $(BUILDFLAGS) $(OBJS) -o $@

$(OBJS): $(OBJDIR)%.o: $(SRCDIR)%.cpp
	$(CPP) -c $(BUILDFLAGS) -I$(HDRDIR) $< -o $@

$(OBJDIR):
	@mkdir -p $@

cleanrelease: $(OBJDIR)
	rm -f $(OBJDIR)*
	rmdir --ignore-fail-on-non-empty $(OBJDIR)

cleandebug: $(OBJDIR)
	rm -f $(OBJDIR)*
	rmdir --ignore-fail-on-non-empty $(OBJDIR)
