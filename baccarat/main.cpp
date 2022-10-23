#include "game.h"

int main(int argc, char *argv[]) {
  std::string mode;
  Game baccarat;
  if (argc > 1) {
    if (std::string(argv[1]) == "--ugly-output") {
      mode = "ugly output";
      baccarat.play(mode);
      return 0;
    } else if ((std::string(argv[1]) == "--without-story")) {
      mode = "without story";
      baccarat.play(mode);
      return 0;
    } else {
    mode = "with story";
    baccarat.play(mode);
    return 0;
  }
  } else {
    mode = "with story";
    baccarat.play(mode);
    return 0;
  }
}


