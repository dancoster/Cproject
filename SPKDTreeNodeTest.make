LIBS=-lm
CC = gcc
OBJS = sp_kd_tree_unit_test.o SPKDTreeNode.o SPPoint.o SPLogger.o SPKDArray.o SPBPriorityQueue.o
EXEC = sp_kd_tree_unit_test
TESTS_DIR = ./unit_tests
COMP_FLAG = -std=c99 -Wall -Wextra \
-Werror -pedantic-errors

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LIBS)
sp_kd_tree_unit_test.o: $(TESTS_DIR)/sp_kd_tree_unit_test.c $(TESTS_DIR)/unit_test_util.h SPKDTreeNode.h
	$(CC) $(COMP_FLAG) -c $(TESTS_DIR)/$*.c
SPKDTreeNode.o: SPKDTreeNode.c SPKDTreeNode.h 
	$(CC) $(COMP_FLAG) -c $*.c
SPPoint.o: SPPoint.c SPPoint.h 
	$(CC) $(COMP_FLAG) -c $*.c
SPLogger.o: SPLogger.c SPLogger.h
	$(CC) $(C_COMP_FLAG) -c $*.c
SPKDArray.o: SPKDArray.c SPKDArray.h SPLogger.h SPPoint.h
	$(CC) $(C_COMP_FLAG) -c $*.c
SPBPriorityQueue.o: SPBPriorityQueue.c SPBPriorityQueue.h
	$(CC) $(C_COMP_FLAG) -c $*.c
clean:
	rm -f $(OBJS) $(EXEC)
