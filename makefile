CC = gcc
CPP = g++
#put all your object files here
OBJS = main.o main_aux.o SPImageProc.o SPPoint.o SPBPriorityQueue.o SPLogger.o SPConfig.o SPKDArray.o SPKDTreeNode.o
#The executabel filename
EXEC = SPCBIR
INCLUDEPATH=/usr/local/lib/opencv-3.1.0/include/
LIBPATH=/usr/local/lib/opencv-3.1.0/lib/
LIBS=-lopencv_xfeatures2d -lopencv_features2d \
-lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_core


CPP_COMP_FLAG = -std=c++11 -Wall -Wextra \
-Werror -pedantic-errors -DNDEBUG

C_COMP_FLAG = -std=c99 -Wall -Wextra \
-Werror -pedantic-errors -DNDEBUG

$(EXEC): $(OBJS)
	$(CPP) $(OBJS) -L$(LIBPATH) $(LIBS) -o $@
main.o: main.cpp main_aux.h
	$(CPP) $(CPP_COMP_FLAG) -I$(INCLUDEPATH) -c $*.cpp
main_aux.o: main_aux.h main_aux.cpp SPKDTreeNode.h SPImageProc.h
	$(CPP) $(CPP_COMP_FLAG) -I$(INCLUDEPATH) -c $*.cpp
#a rule for building a simple c++ source file
#use g++ -MM SPImageProc.cpp to see dependencies
SPImageProc.o: SPImageProc.cpp SPImageProc.h SPConfig.h SPPoint.h SPLogger.h
	$(CPP) $(CPP_COMP_FLAG) -I$(INCLUDEPATH) -c $*.cpp
#a rule for building a simple c source file
#use "gcc -MM SPPoint.c" to see the dependencies
SPPoint.o: SPPoint.c SPPoint.h 
	$(CC) $(C_COMP_FLAG) -c $*.c
SPBPriorityQueue.o: SPBPriorityQueue.c SPBPriorityQueue.h
	$(CC) $(C_COMP_FLAG) -c $*.c
SPLogger.o: SPLogger.c SPLogger.h
	$(CC) $(C_COMP_FLAG) -c $*.c
SPConfig.o: SPConfig.c SPConfig.h SPLogger.h
	$(CC) $(C_COMP_FLAG) -c $*.c
SPKDArray.o: SPKDArray.c SPKDArray.h SPLogger.h SPPoint.h
	$(CC) $(C_COMP_FLAG) -c $*.c
SPKDTreeNode.o: SPKDTreeNode.c SPKDTreeNode.h SPConfig.h SPBPriorityQueue.h SPKDArray.h
	$(CC) $(C_COMP_FLAG) -c $*.c

clean:
	rm -f $(OBJS) $(EXEC)
