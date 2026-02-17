CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wno-missing-field-initializers -Wno-missing-braces
SDL2_INCLUDE = /usr/local/include/SDL2
SDL2_LIB = /usr/local/lib/libSDL2.a
SYS_LIBS = -ldl -lpthread -lrt -lm -lGL -lX11 -lXext -lXrandr -lXi -lXcursor -lXinerama -lasound -lpulse
TARGET = bin/toslim
TERMINAL_TARGET = bin/toslimtool
SRCS = main.cpp
OBJS = $(SRCS:.cpp=.o)
TERMINAL_OBJS = $(SRCS:.cpp=.tool.o)

all: bin $(TARGET)

terminal: CXXFLAGS += -DONLY_TERMINAL
terminal: bin $(TERMINAL_TARGET)

bin:
	mkdir -p bin

$(TARGET): $(OBJS) | bin
	$(CXX) $(CXXFLAGS) -o $@ $^ $(SDL2_LIB) $(SYS_LIBS)

$(TERMINAL_TARGET): $(TERMINAL_OBJS) | bin
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -I$(SDL2_INCLUDE) -c $< -o $@

%.tool.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(TERMINAL_TARGET) $(OBJS) $(TERMINAL_OBJS)

distclean: clean
	rm -rf bin

re: clean all

.PHONY: all clean re distclean terminal