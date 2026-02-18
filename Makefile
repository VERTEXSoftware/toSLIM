CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wno-missing-field-initializers -Wno-missing-braces -O2

INCLUDES = -I./include \
           -I./external/SDL2/include \
           -I./external/SDL2/include/SDL2

SDL2_DIR       = ./external/SDL2
SDL2_BUILD_DIR = $(SDL2_DIR)/build
SDL2_LIB       = $(SDL2_BUILD_DIR)/libSDL2.a
SDL2_INCLUDE   = $(SDL2_DIR)/include

SYS_LIBS = -ldl -lpthread -lrt -lm -lGL -lX11 -lXext -lXrandr -lXi -lXcursor -lXinerama -lasound -lpulse

OBJDIR       = obj
BUILDDIR     = build

TARGET       = $(BUILDDIR)/toslim
TERMINAL_TARGET = $(BUILDDIR)/toslimtool

SRCS         = src/toslim.cpp

OBJS         = $(patsubst %.cpp,$(OBJDIR)/%.o, $(notdir $(SRCS)))
TERMINAL_OBJS = $(patsubst %.cpp,$(OBJDIR)/%.tool.o, $(notdir $(SRCS)))

SDL2_CFLAGS  = -I$(SDL2_INCLUDE) -I$(SDL2_INCLUDE)/SDL2 -D_REENTRANT
SDL2_LDFLAGS = $(SDL2_LIB) $(SYS_LIBS)


all: $(TARGET)

terminal: CXXFLAGS += -DONLY_TERMINAL
terminal: $(TERMINAL_TARGET)

$(TARGET): $(OBJS) $(SDL2_LIB) | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(SDL2_LDFLAGS)
	@echo "Build finish: $@"

$(TERMINAL_TARGET): $(TERMINAL_OBJS) | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -o $@ $(TERMINAL_OBJS)
	@echo "Terminal build finish: $@"


$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)


$(OBJDIR)/%.o: src/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SDL2_CFLAGS) -c $< -o $@


$(OBJDIR)/%.tool.o: src/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SDL2_CFLAGS) -c $< -o $@


$(SDL2_LIB):
	@echo "SDL2 not found. Starting build..."
	@if [ ! -d "$(SDL2_DIR)" ]; then \
		echo "SDL2 directory does not exist: $(SDL2_DIR)"; \
		exit 1; \
	fi
	mkdir -p $(SDL2_BUILD_DIR)
	cd $(SDL2_BUILD_DIR) && cmake .. -DCMAKE_BUILD_TYPE=Release
	cd $(SDL2_BUILD_DIR) && make -j$$(nproc)
	@echo "SDL2 build complete"

clean:
	rm -rf $(OBJDIR)/*.o $(OBJDIR)/*.tool.o $(OBJDIR) $(BUILDDIR)
	rm -f $(TARGET) $(TERMINAL_TARGET)
	@echo "Clean complete"

.PHONY: all clean terminal