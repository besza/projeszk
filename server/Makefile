BIN = bin/server
CXXFLAGS = -Wall -std=c++11 -pedantic
LDFLAGS = -lboost_system
TESTCXXFLAGS = $(CXXFLAGS)
TESTLDFLAGS = -lgtest

CC = g++
SRCS = $(shell find src/ -name "*.cpp")
OBJS = $(patsubst src/%.cpp,obj/%.o,$(SRCS))
DEPS = deps
TESTS = $(shell find test/ -name "*.cpp")
TESTOBJS = $(patsubst test/%.cpp,test/obj/%.o,$(TESTS))
TESTBINS = $(patsubst test/obj/%.o,bin/test_%,$(TESTOBJS))
TESTDEPS = testdeps

.PHONY: all clean tests

all: $(BIN) $(TESTBINS)

clean:
	rm -f $(DEPS) $(OBJS) $(BIN) $(TESTDEPS) $(TESTOBJS) $(TESTBINS)

tests:
	$(foreach x,$(TESTBINS),./$(x) --gtest_color=yes;)

$(BIN): $(OBJS)
	mkdir -p bin
	$(CC) -o $@ $(LDFLAGS) $^

bin/test_%: test/obj/%.o $(filter-out obj/main.o,$(OBJS))
	mkdir -p bin
	$(CC) -o $@ $(LDFLAGS) $(TESTLDFLAGS) $^

obj/%.o: src/%.cpp
	mkdir -p obj
	$(CC) -c -o $@ $(CXXFLAGS) $<

test/obj/%.o: test/%.cpp
	mkdir -p test/obj
	$(CC) -I src -c -o $@ $(TESTCXXFLAGS) $<

$(DEPS): $(SRCS)
	$(CC) -MM $(SRCS) | sed 's/.*/obj\/&/' > $@

$(TESTDEPS):
	$(CC) -I src -MM $(TESTS) | sed 's/.*/test\/obj\/&/' > $@

-include $(DEPS)
-include $(TESTDEPS)
