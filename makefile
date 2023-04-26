CC = gcc
LD = gcc
CFLAGS = -g Wall
LFLAGS = -lm -s
OBJ = main.o linkedList.o
EXEC = main
$(EXEC): $(OBJ)
	$(LD) $(LFLAGS) $(OBJ) -o $(EXEC)

main.o: main.c linkedList.h
	$(CC) -c main.c

linkedList.o: linkedList.c
	$(CC) -c linkedList.c

clean:
	$(RM) $(EXEC) $(OBJ)

val:
	valgrind --leak-check=full -s ./$(EXEC)

# DO NOT DELETE
