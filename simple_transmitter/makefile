#########################################################
# simple transmitter v0.1.2012.9                        #
# Copyright (c) 2012 by X.D. Guo, ALL RIGHTS RESERVED.  #
#########################################################

#* --- define variables --------------------------*#
CC      = gcc
CXX     = g++
TARGET  = transmitter
OBJDIR  = obj
#*** widcard usage: SOURCES = $(wildcard %.cpp)
SOURCES = xsocket.cpp \
          transmission_basic.cpp \
          transmission_client.cpp \
          transmission_server.cpp \
          transmitter.cpp
#*** patsubst: <pattern>, <replacement>, <text>
OBJECTS = $(patsubst %.cpp, $(OBJDIR)/%.o, $(SOURCES))

#*--- specific file serach path ------------------*#
#vpath xsocket.cpp lib

#*--- specific phony targets ---------------------*#
.PHONY: install clean all

#*--- rules --------------------------------------*#
#*** $^: all dependency, whole
#*** $<: all dependency, one by one
#*** $@: all targets, one by one0
$(TARGET): $(OBJECTS)
	$(CXX) $^ -o $@ 

$(OBJDIR)/%.o: %.cpp
	$(CXX) -c $< -o $@

$(OBJDIR)/xsocket.o: common/xsocket.cpp
	$(CXX) -c $^ -o $@

#*--- phony targets -------------------------------*#
install:
	echo installing...
	echo install complete.
clean:
	rm $(TARGET) $(OBJECTS) -f

all:
	make

