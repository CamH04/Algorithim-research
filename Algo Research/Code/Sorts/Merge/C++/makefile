CXX = g++
CXXFLAGS = -std=c++11 -Wall
TARGET = merge_sort_program
OBJ_FILES = main.o

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ_FILES)

main.o: main.cpp merge_sort.h
	$(CXX) $(CXXFLAGS) -c main.cpp

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) $(OBJ_FILES)

