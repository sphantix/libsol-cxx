#*************************************************
# *
# *  Copyright (c) 2015 sphantix 
# *  All Rights Reserved
# *
# * ***********************************************
# #
#
#set your own environment option  
LDFLAGS +=
CFLAGS +=

#make target lib and relevant obj
PRG = crypt.a
OBJS = teacrypt.o

#all target  
all: $(PRG)
$(PRG): $(OBJS)
	$(AR) -rcs $@ $(OBJS)

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) $(CFLAGS) -c $*.cpp -o $*.o

.PRONY:clean
clean:
	@echo "Removing linked and compiled files......"
	rm -f $(OBJS) $(PRG)

#
# Include the rule for making dependency files.
# The '-' in front of the second include suppresses
# error messages when make cannot find the .d files.
# It will just regenerate them.
# See Section 4.14 of Gnu Make.
#

-include $(OBJS:.o=.d)
