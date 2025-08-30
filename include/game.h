#ifndef GAME_H
#define GAME_H

#include "glad/glad.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>
#include "shader.h"
#include "Particle.h"
#include "imgui.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_opengl3.h"

class Game
{
public:
  Game();

  bool init(const char *title, int WINDOW_W, int WINDOW_H);
  void handleEvent();
  void update(float dt);
  void render();
  bool running() { return isRunning; }
  void clear();

  void recreateBuffers();

  void ImguiInit();
  void ImguiRender();

  std::vector<glm::vec3> colors;

  void handleCameraControls(float dt);
  glm::mat4 getViewMatrix() const;
  glm::mat4 getProjectionMatrix() const;

  glm::vec2 cameraPosition;
  float cameraZoom;
  float cameraSpeed;

private:
  int WINDOW_W, WINDOW_H;
  SDL_Window *window;
  SDL_GLContext context;
  bool isRunning;
  Shader *shader;

  Uint64 frameTimePrev;
  int frameCount;
  float fps;

  Particle *p;
  int previousNumParticles;
};

#endif // !GAME_H
