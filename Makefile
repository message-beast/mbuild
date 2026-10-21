CC = gcc
FLAGS = -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables -fomit-frame-pointer -Wall
SOURCE_CODE = *.c
EXECUTABLE_PATH = /usr/bin/mbuild 

compile:
	$(CC) $(SOURCE_CODE) -o $(EXECUTABLE_PATH) $(FLAGS) 
