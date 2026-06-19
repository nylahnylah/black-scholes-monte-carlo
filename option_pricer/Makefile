# Compiler and flags
CXX     = g++
CXXFLAGS = -std=c++17 -Wall -O2 -Iinclude

# Source files and output executable
SRCS = src/main.cpp src/BlackScholes.cpp src/MonteCarlo.cpp
TARGET = pricer

# Default rule — built when you just type 'make'
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

# 'make clean' deletes the compiled executable so you can do a fresh build
clean:
	del $(TARGET).exe
