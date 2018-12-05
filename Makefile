CXX = /media/hdd/linaro/gcc-linaro-7.3.1-2018.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-g++
CXXFLAGS = -std=c++17 -fPIC -Wall -g
LDFLAGS = -shared
INC = -Iinc/
SYSROOT = /media/hdd/linaro/sysroot
LIBS = -lmraa
TARGET = lib/libglassesDevices.so
SOURCES = $(wildcard src/*.cpp)
OBJS = $(SOURCES:.cpp=.o)
INSTALL_DIR = /usr/local/lib

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(INC) $^ -o $@ --sysroot=$(SYSROOT) $(LIBS)

install:
	cp $(TARGET) $(INSTALL_DIR)

uninstall:
	rm $(INSTALL_DIR)/$(TARGET)

clean:
	rm $(TARGET)
	