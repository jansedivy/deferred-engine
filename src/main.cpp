#include "game.h"

int main() {

  Game game;

  while (game.running) {
    game.tick();
  }

  return 0;
}
