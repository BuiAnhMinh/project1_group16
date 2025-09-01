CXX = g++
CXXFLAGS = -std=c++17 -Wall -Werror -O2 -pthread
LDFLAGS = -pthread
TARGETS = mmcopier   # mscopier

all: $(TARGETS)

mmcopier: mmcopier.o file_utils.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGETS) *.o
