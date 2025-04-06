#Set target name
TARGET = home_irrigation_server

#Set source, header, build and bin directories
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
BIN_DIR = bin

#Set compiler and its options
CC = g++
CFLAGS = -std=c++11 -Wall -I$(INCLUDE_DIR) -g
LFLAGS = -lpigpio -lssl -lcrypto -lsqlite3

#Get source code and set object and target files
SRCS = $(shell find $(SRC_DIR) -name '*.cpp')
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))
TARGET_BIN = $(BIN_DIR)/$(TARGET)

#Default target
all: $(TARGET_BIN)

#Generate object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

#Link object files to generate target
$(TARGET_BIN): $(OBJS)
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(BIN_DIR)/keys
	@mkdir -p $(BIN_DIR)/dbs
	@mkdir -p $(BIN_DIR)/temp
	$(CC) $^ $(LFLAGS) -o $@

#Clean build folders
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all clean