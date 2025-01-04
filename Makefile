# compiler
CXX = g++

# compiler flags
CXXFLAGS = -Wall -Wextra -lmpdclient

# target executable
TARGET = build/mpcpp
TARGET_DEL = build/mpcpp

# source files
SRCS = $(wildcard src/*.cpp)
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET) run

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	$(TARGET)

clean:
	rm -f $(TARGET_DEL) $(OBJS)
