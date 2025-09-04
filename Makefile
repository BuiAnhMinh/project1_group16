# Compilers
CXX     := g++

# Flags
CXXFLAGS := -std=c++17 -Wall -Werror -O2 -pthread
LDLIBS   := -pthread -lstdc++fs

# Targets
TARGETS := mmcopier mscopier
MMCOPIER_OBJS := mmcopier.o file_utils.o
MSCOPIER_OBJS := mscopier.o

.PHONY: all clean
all: $(TARGETS)

# Link rules
mmcopier: $(MMCOPIER_OBJS)
	$(CXX) -o $@ $^ $(LDLIBS)

mscopier: $(MSCOPIER_OBJS)
	$(CXX) -o $@ $^ $(LDLIBS)

# Compile rules (handle both C and C++)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGETS) *.o
