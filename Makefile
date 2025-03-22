# 设置项目名称
TARGET = my_project

# 设置源文件目录、头文件目录和构建目录
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
BIN_DIR = bin

# 设置编译器和编译选项
CC = g++
CFLAGS = -std=c++17 -Wall -I$(INCLUDE_DIR)
LFLAGS =

# 自动获取源文件和目标文件
SRCS = $(shell find $(SRC_DIR) -name '*.cpp')
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))
TARGET_BIN = $(BIN_DIR)/$(TARGET)

# 默认目标
all: $(TARGET_BIN)

# 编译规则：生成目标文件
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# 链接目标文件生成可执行文件
$(TARGET_BIN): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(LFLAGS) $^ -o $@

# 清理生成的文件
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# 查看依赖关系
.PHONY: all clean