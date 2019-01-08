SYSROOT = /home/andrew/dev/dragonboard/sysroot

SHELL = /bin/bash
CXX = $(SYSROOT)/../gcc-linaro-7.3.1-2018.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-g++
AR = $(SYSROOT)/../gcc-linaro-7.3.1-2018.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-ar
LD = $(SYSROOT)/../gcc-linaro-7.3.1-2018.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-ld
CXXFLAGS = -std=c++17 -Wall -g
INC = -I. -I./spdlog/include/ -I./libsoc-cpp/
LIBS = -L./libsoc-cpp -Wl,-rpath=./libsoc-cpp -lsoc-cpp
TARGET = libglassesDevices
SOURCES = $(wildcard src/*.cpp)
OBJS = $(SOURCES:.cpp=.o)
INSTALL_DIR = /usr/lib

all: build_libsoc build_dir static shared

build_libsoc:
	+$(MAKE) -C libsoc-cpp

build_dir:
	mkdir -p lib

static: lib/$(TARGET).a
shared: lib/$(TARGET).so

lib/$(TARGET).so: $(SOURCES)
	$(CXX) $(CXXFLAGS) -fPIC -shared $(INC) --sysroot=$(SYSROOT) $^ -o $@ $(LIBS)

lib/$(TARGET).a: $(OBJS)
	$(AR) rvs $@ $(OBJS)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INC) --sysroot=$(SYSROOT) $(LIBS)

install:
	cp lib/$(TARGET).so $(SYSROOT)/$(INSTALL_DIR)
	cp lib/$(TARGET).a $(SYSROOT)/$(INSTALL_DIR)
	sudo -A -s cp lib/$(TARGET).so $(INSTALL_DIR)
	sudo -A -s cp lib/$(TARGET).a $(INSTALL_DIR)

uninstall:
	rm $(INSTALL_DIR)/$(TARGET).so $(INSTALL_DIR)/$(TARGET).a

clean:
	+$(MAKE) clean -C libsoc-cpp
	rm -f $(TARGET).so $(TARGET).a src/*.o
	
cleanobjects:
	rm src/*.o