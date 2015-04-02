DEBUG=yes
GRAPHIC=yes
SIZE=20
CC=g++
CFLAGS=-W -Wall -ansi -pedantic -O0 -std=c++11 -D SIZE=$(SIZE)
LDFLAGS= -lpthread
ifeq ($(DEBUG),yes)
	CFLAGS += -g
endif
ifeq ($(GRAPHIC),yes)
	CFLAGS += -D GRAPHIC `pkg-config gtkmm-3.0 --cflags`
	LDFLAGS += `pkg-config gtkmm-3.0 --libs`
else
	CFLAGS += `pkg-config gtkmm-3.0 --cflags`
	LDFLAGS += `pkg-config gtkmm-3.0 --libs`
endif

EXEC=DLlinux

ROOT_DIR := $(shell pwd)
BIN_DIR = $(ROOT_DIR)/bin
BIN_DOC = $(ROOT_DIR)/doc
SRC_DIR = $(ROOT_DIR)/src
INC_DIR = $(ROOT_DIR)/include
EXISTE := $(wildcard $(BIN_DIR))
EXISTE_DOC := $(wildcard $(BIN_DOC))

SRC= $(wildcard $(SRC_DIR)/*.cpp)
NAME = $(basename $(notdir $(SRC)))
OBJ= $(addprefix $(BIN_DIR)/, $(addsuffix .o, $(NAME)))

all: makedir $(EXEC)
ifeq ($(DEBUG),yes)
	@echo "Génération en mode debug"
else
	@echo "Génération en mode release"
endif

ifeq ($(strip $(EXISTE)),)
makedir:
	mkdir $(BIN_DIR)
else
makedir:
endif

$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

$(BIN_DIR)/main.o: $(INC_DIR)/DLControler.h $(INC_DIR)/DLWindow.h

$(BIN_DIR)/thread.o: $(INC_DIR)/thread.h

$(BIN_DIR)/DLControler.o: $(INC_DIR)/map.h $(INC_DIR)/puce.h $(INC_DIR)/thread.h $(INC_DIR)/DLControler.h

$(BIN_DIR)/case.o: $(INC_DIR)/puce.h $(INC_DIR)/case.h

$(BIN_DIR)/DLWindow.o: $(INC_DIR)/DLMatrixHelper.h $(INC_DIR)/DLControler.h $(INC_DIR)/DLWindow.h

$(BIN_DIR)/map.o: $(INC_DIR)/case.h $(INC_DIR)/puce.h $(INC_DIR)/map.h

$(BIN_DIR)/puce.o: $(INC_DIR)/thread.h $(INC_DIR)/DLControler.h $(INC_DIR)/puce.h

$(BIN_DIR)/thread.o: $(INC_DIR)/thread.h

$(BIN_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) -o $@ -c $< -I$(INC_DIR) $(CFLAGS)

.PHONY: clean mrproper memory_test doc

memory_test: all
	valgrind --leak-check=yes $(ROOT_DIR)/$(EXEC)

ifeq ($(strip $(EXISTE_DOC)),)
makedirdoc:
	mkdir $(BIN_DOC)
else
makedirdoc:
endif

doc: makedirdoc
	doxygen configDoxygen

clean:
	rm -rf $(BIN_DIR)/*.o
	-rmdir $(BIN_DIR)
	rm -rf $(BIN_DOC)/*
	-rmdir $(BIN_DOC)

mrproper: clean
	rm -rf $(EXEC)

install:
	apt-get install build-essential g++ libx11-dev libogre-1.9-dev doxygen
