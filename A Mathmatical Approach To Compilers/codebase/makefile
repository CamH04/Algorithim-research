CXX = g++
AS  = as
LD  = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Iinclude
#for later
ASFLAGS  =
LDFLAGS  =

CPP_SOURCES := $(shell find src -name '*.cpp')
objects := $(CPP_SOURCES:src/%.cpp=obj/%.o)

all: main

main: $(objects)
	$(LD) $(LDFLAGS) -o $@ $(objects)

obj/%.o: src/%.cpp
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

debug: CXXFLAGS += -DDEBUG -g -O0
debug: clean main

release: CXXFLAGS += -O3 -DNDEBUG
release: clean main

run: main
	./main test.lctest

run-example: main example.lctest
	./main example.lctest --stats

run-stats: main
	./main test.lctest --stats
	make clean

example.lctest:
	@echo ";; Comprehensive Lambda Calculus Compiler Test" > example.lctest
	@echo ";; Arithmetic operations" >> example.lctest
	@echo "(+ 2 3)" >> example.lctest
	@echo "(* 4 5)" >> example.lctest
	@echo "(- 10 3)" >> example.lctest
	@echo "" >> example.lctest
	@echo ";; Boolean operations" >> example.lctest
	@echo "(and true false)" >> example.lctest
	@echo "(or true false)" >> example.lctest
	@echo "(not false)" >> example.lctest
	@echo "" >> example.lctest
	@echo ";; Comparisons" >> example.lctest
	@echo "(< 5 10)" >> example.lctest
	@echo "(= 7 7)" >> example.lctest
	@echo "(>= 15 10)" >> example.lctest
	@echo "" >> example.lctest
	@echo ";; Conditional expressions" >> example.lctest
	@echo "(if true 42 0)" >> example.lctest
	@echo "(if (< 3 5) (+ 1 2) (* 2 2))" >> example.lctest
	@echo "" >> example.lctest
	@echo ";; Lambda functions" >> example.lctest
	@echo "(lambda (x) (+ x 1))" >> example.lctest
	@echo "(lambda (x y) (+ x y))" >> example.lctest
	@echo "" >> example.lctest
	@echo ";; Let bindings" >> example.lctest
	@echo "(let ((x 5) (y 3)) (+ x y))" >> example.lctest
	@echo "(let ((double (lambda (n) (* n 2)))) (double 21))" >> example.lctest
	@echo "" >> example.lctest
	@echo ";; Higher-order functions" >> example.lctest
	@echo "((lambda (f x) (f (f x))) (lambda (n) (+ n 1)) 5)" >> example.lctest
	@echo "" >> example.lctest
	@echo ";; List operations" >> example.lctest
	@echo "(cons 1 (cons 2 nil))" >> example.lctest
	@echo "(car (cons 42 nil))" >> example.lctest
	@echo "(cdr (cons 1 (cons 2 nil)))" >> example.lctest
	@echo "" >> example.lctest
	@echo ";; Utility functions" >> example.lctest
	@echo "(succ 5)" >> example.lctest
	@echo "(pred 6)" >> example.lctest
	@echo "(zero? 0)" >> example.lctest
	@echo "(null? nil)" >> example.lctest
	@echo "" >> example.lctest
	@echo ";; Church numeral caching test" >> example.lctest
	@echo "(+ 100 200)" >> example.lctest
	@echo "(* 50 4)" >> example.lctest
	@echo "(+ 100 200)" >> example.lctest

perf-test: main
	@echo ";; Performance test - Large expressions" > perf-test.lctest
	@for i in $$(seq 1 50); do echo "(+ $$i $$(expr $$i + 1))"; done >> perf-test.lctest
	@for i in $$(seq 1 25); do echo "(lambda (x$$i) (+ x$$i $$i))"; done >> perf-test.lctest
	@for i in $$(seq 1 25); do echo "(let ((x $$i)) (+ x $$(expr $$i \* 2)))"; done >> perf-test.lctest
	@echo "Running performance test..."
	./main perf-test.lctest --stats
	@rm -f perf-test.lctest perf-test.lc

memcheck: main example.lctest
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./main example.lctest

test-sizes: main
	@echo "Testing compilation with different input sizes..."
	@echo "(+ 1 2)" > small.lctest
	./main small.lctest --stats
	@make example.lctest
	./main example.lctest --stats
	@make perf-test > /dev/null 2>&1
	./main perf-test.lctest --stats
	@rm -f small.lctest small.lc perf-test.lctest perf-test.lc

test: main example.lctest
	@echo "=== Running Lambda Calculus Compiler Test Suite ==="
	@echo "Test 1: Basic arithmetic"
	@echo "(+ 2 3)" > test1.lctest
	./main test1.lctest
	@echo "✓ Basic arithmetic passed"

	@echo "Test 2: Lambda functions"
	@echo "(lambda (x) (+ x 1))" > test2.lctest
	./main test2.lctest
	@echo "✓ Lambda functions passed"

	@echo "Test 3: Let bindings"
	@echo "(let ((x 5)) (+ x 1))" > test3.lctest
	./main test3.lctest
	@echo "✓ Let bindings passed"

	@echo "Test 4: Conditionals"
	@echo "(if true 1 0)" > test4.lctest
	./main test4.lctest
	@echo "✓ Conditionals passed"

	@echo "Test 5: Comprehensive test"
	./main example.lctest
	@echo "✓ Comprehensive test passed"

	@rm -f test*.lctest test*.lc
	@echo "=== All tests passed! ==="

format:
	@if command -v clang-format >/dev/null 2>&1; then \
		find src include -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i; \
		echo "Code formatted successfully"; \
	else \
		echo "clang-format not found, skipping formatting"; \
	fi

analyze:
	@if command -v cppcheck >/dev/null 2>&1; then \
		cppcheck --enable=all --std=c++17 --suppress=missingIncludeSystem src/; \
		echo "Static analysis completed"; \
	else \
		echo "cppcheck not found, skipping analysis"; \
	fi

info:
	@echo "=== Build Configuration ==="
	@echo "Compiler: $(CXX)"
	@echo "Flags: $(CXXFLAGS)"
	@echo "Sources found:"
	@for src in $(CPP_SOURCES); do echo "  $$src"; done
	@echo "Objects:"
	@for obj in $(objects); do echo "  $$obj"; done
	@echo "=========================="

help:
	@echo "Lambda Calculus Compiler - Make Targets:"
	@echo ""
	@echo "Building:"
	@echo "  all (default) - Build the compiler"
	@echo "  main          - Build the compiler (same as all)"
	@echo "  debug         - Build with debug symbols"
	@echo "  release       - Build optimized release version"
	@echo ""
	@echo "Testing:"
	@echo "  run           - Run with test.lctest"
	@echo "  run-stats     - Run with test.lctest and show stats"
	@echo "  run-example   - Run with comprehensive example"
	@echo "  test          - Run full test suite"
	@echo "  test-sizes    - Test with different input sizes"
	@echo "  perf-test     - Performance test with large input"
	@echo "  memcheck      - Run with Valgrind (if available)"
	@echo ""
	@echo "Utilities:"
	@echo "  example.lctest - Create comprehensive test file"
	@echo "  format        - Format code (if clang-format available)"
	@echo "  analyze       - Static analysis (if cppcheck available)"
	@echo "  info          - Show build configuration"
	@echo "  clean         - Remove build artifacts"
	@echo "  help          - Show this help"

.PHONY: clean all debug release run run-stats run-example test test-sizes perf-test memcheck format analyze info help
clean:
	rm -rf obj main example.lctest
