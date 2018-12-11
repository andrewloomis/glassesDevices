SHELL = /bin/bash
CXX = /media/hdd/linaro/gcc-linaro-7.3.1-2018.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-g++
AR = /media/hdd/linaro/gcc-linaro-7.3.1-2018.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-ar
LD = /media/hdd/linaro/gcc-linaro-7.3.1-2018.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-ld
CXXFLAGS = -std=c++17 -Wall -g
INC = -I. -I./spdlog/include/
SYSROOT = /media/hdd/linaro/sysroot
LIBS = -lmraa -pthread
TARGET = lib/libglassesDevices
SOURCES = $(wildcard src/*.cpp)
OBJS = $(SOURCES:.cpp=.o)
INSTALL_DIR = /usr/lib

all: build_dir $(TARGET).so $(TARGET).a

build_dir:
	if [ ! -d "lib" ]; then mkdir lib; fi

static: $(TARGET).a
shared: $(TARGET).so

$(TARGET).so: $(SOURCES)
	$(CXX) $(CXXFLAGS) -fPIC -shared $(INC) --sysroot=$(SYSROOT) $^ -o $@ $(LIBS)

$(TARGET).a: $(OBJS)
	$(AR) rvs $@ $(OBJS)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INC) --sysroot=$(SYSROOT) $(LIBS)

install:
	cp $(TARGET).so $(INSTALL_DIR)
	cp $(TARGET).a $(INSTALL_DIR)

uninstall:
	rm $(INSTALL_DIR)/$(TARGET).so $(INSTALL_DIR)/$(TARGET).a

clean:
	rm $(TARGET).so $(TARGET).a src/*.o
	
cleanobjects:
	rm src/*.o