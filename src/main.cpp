#include "Game.h"

Game game; 

int main(int argc, char *argv[]) {
    
    game.init("Whack-a-Mole", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, false);

    while (game.running()) {
        game.handleEvents();
        game.update();
        game.render();
        
        SDL_Delay(16);
    }

    game.clean();
    return 0;
}