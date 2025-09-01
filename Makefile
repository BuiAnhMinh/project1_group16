
CXX = g++
CXXFLAGS = -Wall -Werror -pthread
TARGETS = mmcopier mscopier
OBJS = mmcopier.o mscopier.o

# Default target: build everything
all: $(TARGETS)

# Build mmcopier
mmcopier: mmcopier.o
	$(CXX) $(CXXFLAGS) -o $@ $^

# Build mscopier
mscopier: mscopier.o
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile .cpp files into .o object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(TARGETS) $(OBJS)