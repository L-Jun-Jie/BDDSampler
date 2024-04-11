# 当前文件夹路径
SRC_DIR := $(CURDIR)
# 工作文件夹路径
WORK_DIR := $(shell dirname $(SRC_DIR))
# 编译文件夹
BUILD_DIR := $(SRC_DIR)/build
# CUDD路径
CUDD_DIR := $(WORK_DIR)/lib/cudd-3.0.0

# 编译器和链接器
CXX := g++-11
LD := g++-11

# 编译器标志
CXXFLAGS := -g -Ofast -Wall -static \
			-I$(WORK_DIR)/lib/cudd-3.0.0/cplusplus \
			-I$(WORK_DIR)/lib/cudd-3.0.0/cudd \
			-I$(WORK_DIR)/lib

# 链接器标志，包括库的搜索路径和链接的库
LDFLAGS := -L $(CUDD_DIR)/cudd/.libs -lcudd

# 自动获取源文件列表和目标文件列表
SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS := $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
TARGET := my_program

# 默认目标
all: $(TARGET)

# 如何生成最终的可执行文件
$(TARGET): $(OBJECTS)
	$(LD) $(OBJECTS) -o $@ $(LDFLAGS)

# 如何从.cpp文件生成.o文件
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 创建 build 目录
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# 清除生成的文件
clean:
	rm -rf $(BUILD_DIR)
