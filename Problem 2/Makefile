ODIR=obj
SRC = src
INC = include
CC=gcc
FLAGS=-I$(INC)

ifeq ($(OS),Windows_NT)
	DelCom=del
else
	DelCom=rm
endif

_OBJ = utils.o solver.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

directories:
	mkdir -p $(ODIR)

$(ODIR)/%.o: $(SRC)/%.c
	$(CC) -c -o $@ $^ $(FLAGS)

prob2: prob2.c $(OBJ)
	$(CC) -o $@ $^ $(FLAGS)

all: directories prob2

.PHONY: clean directories

clean:
	$(DelCom) -f $(ODIR)/*.o
	$(DelCom) prob2