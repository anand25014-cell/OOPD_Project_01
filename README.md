CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread -Iinclude

SRC = src/CellTower.cpp src/CellularCore.cpp src/Protocol.cpp \
      src/UserDevice.cpp src/basicIO.cpp src/CsvReader.cpp src/main.cpp

RELEASE_OBJ = $(patsubst src/%.cpp, build/release/%.o, $(SRC))
DEBUG_OBJ   = $(patsubst src/%.cpp, build/debug/%.o, $(SRC))

all: bin/simulator

debug: bin/simulator_debug

bin/simulator: $(RELEASE_OBJ) build/release/syscall.o
	mkdir -p bin
	$(CXX) $(CXXFLAGS) -O2 -o bin/simulator $(RELEASE_OBJ) build/release/syscall.o

bin/simulator_debug: $(DEBUG_OBJ) build/debug/syscall.o
	mkdir -p bin
	$(CXX) $(CXXFLAGS) -g -O0 -DDEBUG -o bin/simulator_debug $(DEBUG_OBJ) build/debug/syscall.o

# Build rules
build/release/%.o: src/%.cpp
	mkdir -p build/release
	$(CXX) $(CXXFLAGS) -O2 -c $< -o $@

build/debug/%.o: src/%.cpp
	mkdir -p build/debug
	$(CXX) $(CXXFLAGS) -g -O0 -DDEBUG -c $< -o $@

build/release/syscall.o: syscall.S
	mkdir -p build/release
	$(CXX) -c syscall.S -o build/release/syscall.o

build/debug/syscall.o: syscall.S
	mkdir -p build/debug
	$(CXX) -DDEBUG -c syscall.S -o build/debug/syscall.o

clean:
	rm -rf build bin
