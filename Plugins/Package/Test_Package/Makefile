CXX = g++
CXXFLAGS = -shared -fPIC -Wall

TARGET_LINUX = Test_Package.so
TARGET_WINDOWS = Test_Package.dll
SRCS = ./src/Main.cpp
OBJDIR = ./obj
OBJS = $(SRCS:./src/%.cpp=$(OBJDIR)/%.o)

ifeq ($(OS),Windows_NT)
	TARGET = $(TARGET_WINDOWS)
	COMPILER = x86_64-w64-mingw32-g++
	LINKFLAGS = -lmingw32
else
	TARGET = $(TARGET_LINUX)
	COMPILER = $(CXX)
	LINKFLAGS =
endif

all: $(TARGET)

$(TARGET): $(OBJS)
	$(COMPILER) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LINKFLAGS)

$(OBJDIR)/%.o: ./src/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

install: all
	@mkdir -p ../../../Resources/Modules/Package/Test_Package
	cp $(TARGET) ../../../Resources/Modules/Package/Test_Package/$(TARGET)

clean:
	rm -f $(TARGET_LINUX) $(TARGET_WINDOWS) $(OBJDIR)/*.o
	rm -rf $(OBJDIR)

.PHONY: all install clean
