DEBUG=yes
SIZE=20
CC=g++
CFLAGS=-W -Wall -ansi -pedantic -O0 -std=c++11 -D SIZE=$(SIZE)
LDFLAGS= 
ifeq ($(DEBUG),yes)
	CFLAGS += -g
endif

CFLAGS +=  `pkg-config gtkmm-3.0 --cflags`
LDFLAGS += -lX11 -lOgreOverlay -lOgreMain -lboost_system `pkg-config gtkmm-3.0 --libs`

EXEC=FuzzyLogic

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

$(BIN_DIR)/main.o: $(INC_DIR)/ogrewindow.h

$(BIN_DIR)/ogrewindow.o: $(INC_DIR)/ogrewindow.h $(INC_DIR)/ogrewidget.h

$(BIN_DIR)/ogrewidget.o: $(INC_DIR)/ogrewidget.h

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
