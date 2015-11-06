#*************************************************
# *
# *  Copyright (c) 2015 sphantix 
# *  All Rights Reserved
# *
# * ***********************************************
# #
#
#set your own environment option  
CXX = g++
AR = ar

CURR_DIR := $(shell pwd)
MACRO_DEFINES =
HDR_INC = -I$(CURR_DIR) \
		  -I$(CURR_DIR)/include \
		  -I$(CURR_DIR)/include/crypt \
		  -I$(CURR_DIR)/include/basic \

LDFLAGS =
CFLAGS = -g -fPIC -Wall -Werror -Wfatal-errors $(HDR_INC) $(LDFLAGS) $(MACRO_DEFINES)
export CXX
export AR
export CFLAGS
export LDFLAGS 

LIB_DIR=/usr/lib
LIB_INCLUDE_DIR=/usr/include/libsol-cxx
CRYPTDIR = $(CURR_DIR)/crypt
BASICDIR = $(CURR_DIR)/basic

#make target lib and relevant obj, sol is short for sphantix's own lib
PRG = libsol-cxx.so
OBJS = 
INNER_LIBS = crypt/crypt.a
OUTTER_LIBS = 

#all target
all: $(PRG)
$(PRG): $(INNER_LIBS) $(OBJS)
	$(CXX) $(CFLAGS) -shared -Wl,--whole-archive,-soname,$@ -o $@ \
	      $(OBJS) $(CRYPTDIR)/crypt.a \
	      -Wl,-lcrypt -Wl,--no-whole-archive

#basic/basic.a:
#	$(MAKE) -C basic 

crypt/crypt.a:
	$(MAKE) -C crypt 

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) $(CFLAGS) -c $*.cpp -o $*.o

.PRONY:clean install
clean:
	#@$(MAKE) clean -C basic 
	@$(MAKE) clean -C crypt 
	rm -f $(OBJS) $(PRG)

install:$(PRG)
	@install -m 0755 $(PRG) $(LIB_DIR)
	@install -d $(LIB_INCLUDE_DIR)
	@cp -rf ./include/* $(LIB_INCLUDE_DIR)

#
# Include the rule for making dependency files.
# The '-' in front of the second include suppresses
# error messages when make cannot find the .d files.
# It will just regenerate them.
# See Section 4.14 of Gnu Make.
#

-include $(OBJS:.o=.d)
