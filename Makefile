SYSROOT = /home/andrew/dev/dragonboard/sysroot

SHELL = /bin/bash
CXX = $(SYSROOT)/../gcc-linaro-7.3.1-2018.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-g++
AR = $(SYSROOT)/../gcc-linaro-7.3.1-2018.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-ar
LD = $(SYSROOT)/../gcc-linaro-7.3.1-2018.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-ld
CXXFLAGS = -std=c++17 -Wall -g
INC = -I. -I./spdlog/include/ -I./libsoc-cpp/
LIBS = -L./libsoc-cpp -Wl,-rpath=./libsoc-cpp -lsoc-cpp -pthread #-L$(SYSROOT)/usr/lib/aarch64-linux-gnu/ -l:libsoc.so.2
TARGET = lib/libglassesDevices
SOURCES = $(wildcard src/*.cpp)
OBJS = $(SOURCES:.cpp=.o)
INSTALL_DIR = /usr/lib

all: build_libsoc build_dir $(TARGET).so $(TARGET).a

build_libsoc:
	+$(MAKE) -C libsoc-cpp

build_dir:
	mkdir -p lib

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
	+$(MAKE) clean -C libsoc-cpp
	rm $(TARGET).so $(TARGET).a src/*.o
	
cleanobjects:
	rm src/*.o