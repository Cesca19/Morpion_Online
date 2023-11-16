#include "Game.h"

int main()
{
	Game* game = new Game();
	game->init("Morpion", 800, 600);
	game->run();
	return 0;
}