CXX = g++
AS  = as
LD  = g++

CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Iinclude
ASFLAGS  =
LDFLAGS  =

CPP_SOURCES := $(shell find src -name '*.cpp')

objects := $(CPP_SOURCES:src/%.cpp=obj/%.o)

main: $(objects)
	$(LD) $(LDFLAGS) -o $@ $(objects)

obj/%.o: src/%.cpp
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

run: main
	./main test.lctest
	make clean

.PHONY: clean
clean:
	rm -rf obj main
