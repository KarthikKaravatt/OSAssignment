CC = gcc
LD = gcc
CFLAGS = -g 
LFLAGS = -lm -s
OBJ = main.o linkedList.o assignmentMethods.o
EXEC = main
$(EXEC): $(OBJ)
	$(LD) $(CFLAGS) $(OBJ) -o $(EXEC)  

main.o: main.c linkedList.h assignmentMethods.h
	$(CC) -c main.c $(CFLAGS)

linkedList.o: linkedList.c
	$(CC) -c linkedList.c $(CFLAGS)

assignmentMethods.o: assignmentMethods.c linkedList.h
	$(CC) -c assignmentMethods.c $(CFLAGS)

clean:
	$(RM) $(EXEC) $(OBJ)

val:
	valgrind --leak-check=full -s ./$(EXEC)
run:
	./main 9 1 1 1 1 
gdb:
	gdb --args main 2 1 1 1 1 

# DO NOT DELETE
