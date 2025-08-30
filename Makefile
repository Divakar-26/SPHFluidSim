all:
	g++ src/*.cpp src/glad.c \
	    imgui/imgui*.cpp \
	    imgui/backends/imgui_impl_sdl3.cpp \
	    imgui/backends/imgui_impl_opengl3.cpp \
	    -I/usr/local/include -L/usr/local/lib \
	    -Iinclude -Iimgui -Iimgui/backends \
	    -lSDL3 -lGL -ldl -lpthread \
	    -o main
