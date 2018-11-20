COMPILER=g++ -std=c++11
COMPILER_FLAGS= -O0  -g  -Wall -Wextra
COMPILER_FLAGS += -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-parameter -Wno-strict-aliasing  -Wno-sign-compare -Wno-unused-function -Wno-comment
LINKERFLAGS= -L./ -pthread

CWD_ROOT= ./

APPNAME=sensor_test


INC = -I$(CWD_ROOT)
#INC += -I$(PRJ_ROOT)device/pcie_dev_ctrl/include
#INC += -I$(PRJ_ROOT)device/common/include

SRC = $(CWD_ROOT)serial.cpp
SRC += $(CWD_ROOT)sensor.cpp
SRC += $(CWD_ROOT)sensor_daemon.cpp
SRC += $(CWD_ROOT)emulator.cpp
SRC += $(CWD_ROOT)client.cpp




all:
	$(COMPILER) -o $(APPNAME) $(COMPILER_FLAGS) $(LINKERFLAGS)  $(INC) $(SRC)
