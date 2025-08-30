#include "game.h"

Game game;

int main(int argc, char *argv[])
{
  game.init("hello", 1280, 720);
  Uint64 frameTimePrev = SDL_GetTicks();
  while (game.running())
  {
    game.handleEvent();

    Uint64 currentTime = SDL_GetTicks();
    float dt = (currentTime - frameTimePrev) / 1000.0f; // seconds
    frameTimePrev = currentTime;
    
    game.update(dt);

    game.render();
  }

  game.clear();
  return 0;
}
