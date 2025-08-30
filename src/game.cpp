#include "game.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <iostream>
#include <time.h>
#include <vector>

#define UNITMULTIPLIER 100

/*
  1 UNIT Is 100 pixels
  we only do this for rendering and not for calculation of physics
*/

GLuint shaderProgram;
GLuint VBO, VAO;

// defines
float gravity = 980;
float damping = 0.5;

int numOfParticels;

GLuint compileShader(GLenum type, const char *source)
{
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, NULL);
  glCompileShader(shader);
  return shader;
}

Game::Game()
{
  p = new Particle();
  numOfParticels = numOfParticels;
}

bool Game::init(const char *title, int WINDOW_W, int WINDOW_H)
{
  this->WINDOW_W = WINDOW_W;
  this->WINDOW_H = WINDOW_H;

  if (SDL_Init(SDL_INIT_VIDEO) == 0)
  {
    std::cerr << "Failes to initialize sdl" << std::endl;
    return false;
  }
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  window = SDL_CreateWindow(title, WINDOW_W, WINDOW_H, SDL_WINDOW_OPENGL);

  context = SDL_GL_CreateContext(window);

  if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
  {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    return false;
  }

  SDL_GL_SetSwapInterval(0);

  ImguiInit();

  glEnable(GL_PROGRAM_POINT_SIZE); // Enable gl_PointSize in shader
  glEnable(GL_POINT_SPRITE);       // Enable point sprites

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, numOfParticels * sizeof(glm::vec2), p->GetPositions().data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void *)0);
  glEnableVertexAttribArray(0);

  shader = new Shader("shaders/vertex.vert", "shaders/fragment.frag");

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  frameTimePrev = SDL_GetTicks();
  frameCount = 0;
  fps = 0.0f;

  isRunning = true;
  return true;
}

void Game::update(float dt)
{

  p->update(dt);

  numOfParticels = p->GetPositions().size();
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  static size_t lastSize = 0;

  // Reallocate buffer if new particles were added
  if (numOfParticels != lastSize)
  {
    glBufferData(GL_ARRAY_BUFFER, numOfParticels * sizeof(glm::vec2), p->GetPositions().data(), GL_DYNAMIC_DRAW);
    lastSize = numOfParticels;
  }
  else
  {
    glBufferSubData(GL_ARRAY_BUFFER, 0, numOfParticels * sizeof(glm::vec2), p->GetPositions().data());
  }
}

void Game::handleEvent()
{
  SDL_Event event;
  while (SDL_PollEvent(&event))
  {
    ImGui_ImplSDL3_ProcessEvent(&event);

    if (event.type == SDL_EVENT_QUIT)
    {
      isRunning = false;
    }
    if (event.type == SDL_EVENT_KEY_DOWN)
    {
      if (event.key.key == SDLK_UP)
      {
        p->GetRadius() += 1.0f;
      }
    }
  }
}

void Game::render()
{
  frameCount++;
  Uint64 currentTime = SDL_GetTicks();
  Uint64 deltaTime = currentTime - frameTimePrev;

  // Update FPS every second
  if (deltaTime >= 1000)
  {
    fps = frameCount * 1000.0f / deltaTime;
    std::cout << "FPS: " << fps << std::endl;
    std::cout << "Number of particles : " << numOfParticels << std::endl;
    frameCount = 0;
    frameTimePrev = currentTime;
  }

  // 1. Clear screen first
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // 2. Draw your particles
  shader->use();
  shader->setFloat("pointSize", p->GetRadius());
  shader->setVec2("screenSize", glm::vec2(WINDOW_W, WINDOW_H));
  shader->setScale("worldScale", UNITMULTIPLIER);
  glUniform1f(glGetUniformLocation(shader->ID, "uAlpha"), p->alpha); // set alpha = 0.3

  glBindVertexArray(VAO);
  glDrawArrays(GL_POINTS, 0, numOfParticels);

  ImguiRender();

  // 6. Swap
  SDL_GL_SwapWindow(window);
}

void Game::clear()
{
  shader->destroy();

  delete shader;
  SDL_GL_DestroyContext(context);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void Game::ImguiInit()
{
  // imgui Setup
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  ImGui::StyleColorsDark();

  // 2. Init backends
  ImGui_ImplSDL3_InitForOpenGL(window, context);
  ImGui_ImplOpenGL3_Init("#version 330");
}

void Game::ImguiRender()
{
  // 3. Start ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();

  // 4. Build UI
  ImGui::Begin("Debug");
  ImGui::Text("Hello from ImGui!");
  ImGui::SliderFloat("Gravity", &gravity, -20.0f, 20.0f);
  ImGui::SliderFloat("Damping", &damping, 0.0f, 1.0f);
  ImGui::SliderFloat("Radius", &p->GetRadius(), 0.0f, 10.0f);
  ImGui::SliderFloat("alpha", &p->alpha, 0.0f, 1.0f);
  ImGui::Checkbox("start", &p->running);
  ImGui::End();

  // 5. Render ImGui on top
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}