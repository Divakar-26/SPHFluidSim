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
GLuint colorVBO;

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
  numOfParticels = 0;
  previousNumParticles = 0;
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
  glViewport(0, 0, WINDOW_W, WINDOW_H);

  p = new Particle(WINDOW_W, WINDOW_H);
  ImguiInit();

  glEnable(GL_PROGRAM_POINT_SIZE);
  glEnable(GL_POINT_SPRITE);

  numOfParticels = p->GetPositions().size();
  colors.resize(numOfParticels, glm::vec3(0.0f, 0.0f, 1.0f));

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &colorVBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, numOfParticels * sizeof(glm::vec2), p->GetPositions().data(), GL_DYNAMIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void *)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
  glBufferData(GL_ARRAY_BUFFER, numOfParticels * sizeof(glm::vec3), colors.data(), GL_DYNAMIC_DRAW);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);

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

  if (numOfParticels != previousNumParticles || previousNumParticles == -1)
  {
    std::cout << "Particle count changed from " << previousNumParticles
              << " to " << numOfParticels << std::endl;
    recreateBuffers();
  }

  previousNumParticles = numOfParticels;

  colors.resize(numOfParticels);
  float maxSpeed = 5.0f;

  for (int i = 0; i < numOfParticels; i++)
  {
    float t = glm::clamp(p->speed[i] / maxSpeed, 0.0f, 1.0f);
    colors[i] = glm::vec3(t, 0.0f, 1.0f - t);
  }

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferSubData(GL_ARRAY_BUFFER, 0, numOfParticels * sizeof(glm::vec2), p->GetPositions().data());

  glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
  glBufferSubData(GL_ARRAY_BUFFER, 0, numOfParticels * sizeof(glm::vec3), colors.data());
}

void Game::handleEvent()
{
  SDL_Event event;
  ImGuiIO &io = ImGui::GetIO();

  while (SDL_PollEvent(&event))
  {
    ImGui_ImplSDL3_ProcessEvent(&event);
    if (io.WantCaptureMouse)
    {
      switch (event.type)
      {
      case SDL_EVENT_MOUSE_BUTTON_DOWN:
      case SDL_EVENT_MOUSE_BUTTON_UP:
      case SDL_EVENT_MOUSE_MOTION:
      case SDL_EVENT_MOUSE_WHEEL:
        continue; 
      }
    }

    if (io.WantCaptureKeyboard)
    {
      switch (event.type)
      {
      case SDL_EVENT_KEY_DOWN:
      case SDL_EVENT_KEY_UP:
      case SDL_EVENT_TEXT_INPUT:
        continue; 
      }
    }

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

    p->OnEvent(event);
  }
}

void Game::render()
{
  frameCount++;
  Uint64 currentTime = SDL_GetTicks();
  Uint64 deltaTime = currentTime - frameTimePrev;

  if (deltaTime >= 1000)
  {
    fps = frameCount * 1000.0f / deltaTime;
    std::cout << "FPS: " << fps << std::endl;
    std::cout << "Number of particles : " << numOfParticels << std::endl;
    frameCount = 0;
    frameTimePrev = currentTime;
  }

  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  shader->use();
  shader->setFloat("pointSize", p->GetRadius() * 2.0f);
  shader->setVec2("screenSize", glm::vec2(WINDOW_W, WINDOW_H));
  shader->setScale("worldScale", UNITMULTIPLIER);
  glUniform1f(glGetUniformLocation(shader->ID, "uAlpha"), p->alpha); // set alpha = 0.3

  glBindVertexArray(VAO);
  glDrawArrays(GL_POINTS, 0, numOfParticels);

  ImguiRender();

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

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  ImGui::StyleColorsDark();

  ImGui_ImplSDL3_InitForOpenGL(window, context);
  ImGui_ImplOpenGL3_Init("#version 330");
}

void Game::ImguiRender()
{

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();

  ImGui::Begin("Debug");
  ImGui::Text("Hello from ImGui!");
  ImGui::SliderFloat("Gravity", &p->GRAVITY, 0.0f, 100.0f);
  // ImGui::SliderFloat("Damping", &p->damping, 0.0f, 3.0f);
  if (ImGui::SliderFloat("Radius", &p->GetRadius(), 0.0f, 10.0f))
    ;
  ImGui::SliderFloat("alpha", &p->alpha, 0.0f, 1.0f);

  if (ImGui::SliderInt("numParticles", &p->numParticles, 0, 3500) ||
      ImGui::SliderFloat("spacing", &p->particleSpacing, 0.0f, 2.0f))
  {
    p->MakeGrid();
    previousNumParticles = -1;
  }

  ImGui::SliderFloat("smoothign Radius", &p->smoothingRadius, 0.0f, 10.0f);
  ImGui::SliderFloat("target Density", &p->targetDensity, 0.0f, 20.0f);
  ImGui::SliderFloat("pressureMultiplier", &p->pressureMultiplier, 0.0f, 200.0f);
  ImGui::Checkbox("start", &p->running);
  ImGui::End();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Game::recreateBuffers()
{

  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &colorVBO);
  glDeleteVertexArrays(1, &VAO);

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &colorVBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, numOfParticels * sizeof(glm::vec2), p->GetPositions().data(), GL_DYNAMIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void *)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
  glBufferData(GL_ARRAY_BUFFER, numOfParticels * sizeof(glm::vec3), colors.data(), GL_DYNAMIC_DRAW);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
}