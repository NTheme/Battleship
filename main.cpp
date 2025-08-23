#include <iostream>

#include "lib/game.hpp"

int main() {
  GameLoop game(Vector2u(10, 10), 2);
  game.Go();
  return 0;
}
