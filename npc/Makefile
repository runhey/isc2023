TOPNAME = top  # 现在还不知道干啥的, 好像是设置输入的.v文件名字
NXDC_FILES = constr/top.nxdc  # 引脚对应
INC_PATH ?=

# 构造输出产物
BUILD_DIR = ./build
OBJ_DIR = $(BUILD_DIR)/obj_dir
BIN = $(BUILD_DIR)/$(TOPNAME)


ifeq ($(VERILATOR_ROOT),)
VERILATOR = verilator
else
export VERILATOR_ROOT
VERILATOR = $(VERILATOR_ROOT)/bin/verilator
endif
# 默认的编译参数
VERILATOR_CFLAGS += -MMD --build -cc  \
				-O3 --x-assign fast --x-initial fast --noassert

# 为什么必须要在这个位置
default: 
	@echo "请输入编译目标"

$(shell mkdir -p $(BUILD_DIR))
# constraint file
SRC_AUTO_BIND = $(abspath $(BUILD_DIR)/auto_bind.cpp)
$(SRC_AUTO_BIND): $(NXDC_FILES)
	python3 $(NVBOARD_HOME)/scripts/auto_pin_bind.py $^ $@
# project source
VSRCS = $(shell find $(abspath ./vsrc) -name "*.v")
CSRCS = $(shell find $(abspath ./csrc) -name "*.c" -or -name "*.cc" -or -name "*.cpp")
CSRCS += $(SRC_AUTO_BIND)
# rules for NVBoard
include $(NVBOARD_HOME)/scripts/nvboard.mk
# rules for verilator
INCFLAGS = $(addprefix -I, $(INC_PATH))
CXXFLAGS += $(INCFLAGS) -DTOP_NAME="\"V$(TOPNAME)\""

# BIN
# 依赖 VSRCS CSRCS NVBOARD_ARCHIVE
# 1. 执行 verilator + 参数
# 2. 指定顶层模块名 + 表示依赖文件列表
# 3. 添加编译标志“-CFLAGS” 和链接标志“-LDFLAGS”
# 4. 指定输出目录和输出目标
$(BIN): $(VSRCS) $(CSRCS) $(NVBOARD_ARCHIVE)
	@rm -rf $(OBJ_DIR)
	$(VERILATOR) $(VERILATOR_CFLAGS) \
		--top-module $(TOPNAME) $^ \
		$(addprefix -CFLAGS , $(CXXFLAGS)) $(addprefix -LDFLAGS , $(LDFLAGS)) \
		--Mdir $(OBJ_DIR) --exe -o $(abspath $(BIN))

do_build = $(VERILATOR) $(VERILATOR_CFLAGS) \
		--top-module $(TOPNAME) $^ \
		$(addprefix -CFLAGS , $(CXXFLAGS)) $(addprefix -LDFLAGS , $(LDFLAGS)) \
		--Mdir $(OBJ_DIR) --exe -o $(abspath $(BIN))


# default 依赖于 BIN
# 1. 生成目录
# 2. 生成bind pin cpp文件
# 3. 寻找所有的 .v .cpp .c 文件
# 4. 包含 nvboard.mk
# 5. 设置verilator 开关变量
# 6. 正式执行 构建


all: default

run: $(BIN)
	@$^

# 最开始的示例，现在留着
sim:
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	@echo "Write this Makefile by your self."
	$(VERILATOR) -cc --exe --build -j vsrc/top.v csrc/main_sim.cpp --trace

clean:
	rm -rf $(BUILD_DIR)
	$(call git_commit, "sim RTL")

VSRCS_DOUBLE_SWITCH = ./vsrc/double_switch.v 
CSRCS_DOUBLE_SWITCH = $(abspath ./csrc/double_switch.cpp) 
CSRCS_DOUBLE_SWITCH += $(SRC_AUTO_BIND)

double_switch: $(VSRCS_DOUBLE_SWITCH) $(CSRCS_DOUBLE_SWITCH) $(NVBOARD_ARCHIVE)
	@rm -rf $(OBJ_DIR)
	$(call git_commit, "sim RTL")
	$(do_build)

VSRCS_LIGHT = $(abspath ./vsrc/light.v) 
CSRCS_LIGHT = $(abspath ./csrc/light.cpp) 
CSRCS_LIGHT += $(SRC_AUTO_BIND)
light: $(VSRCS_LIGHT) $(CSRCS_LIGHT) $(NVBOARD_ARCHIVE)
	@rm -rf $(OBJ_DIR)
	$(call git_commit, "SIM RTL light")
	$(do_build)








.PHONY: default all clean run double_switch light

include ../Makefile
