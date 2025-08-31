CXX = g++
CXXFLAGS = -Iinclude \
           -Ivendor/glad/include \
           -Ivendor/glm \
           -Ivendor/SDL3/include \
           -Ivendor/imgui \
           -Ivendor/imgui/backends
LDFLAGS = -Lvendor/SDL3/build \
          -lSDL3 -lGL -ldl -lpthread

SRC = src/*.cpp src/glad.c \
      vendor/imgui/imgui*.cpp \
      vendor/imgui/backends/imgui_impl_sdl3.cpp \
      vendor/imgui/backends/imgui_impl_opengl3.cpp

all:
	$(CXX) $(SRC) $(CXXFLAGS) $(LDFLAGS) -o main
