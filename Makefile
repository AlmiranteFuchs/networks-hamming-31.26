# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -O2 -Isrc

# Source files
SRC = main.cpp src/encoder.cpp src/decoder.cpp src/simulate.cpp src/hamming.cpp
OBJ = $(SRC:.cpp=.o)

# Output binary
TARGET = hamming3126

# Default target
all: $(TARGET)

# Link object files into the final binary
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile .cpp to .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJ) $(TARGET)

# Run program
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
