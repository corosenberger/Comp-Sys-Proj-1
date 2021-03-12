
CC=gcc
FLAGS=

ifeq ($(OS),Windows_NT)
	DelCom=del
else
	DelCom=rm
endif

filegen: filegen.c 
	$(CC) -o $@ $^ $(FLAGS)

all: filegen

.PHONY: clean 

clean:
	$(DelCom) Problem1.o