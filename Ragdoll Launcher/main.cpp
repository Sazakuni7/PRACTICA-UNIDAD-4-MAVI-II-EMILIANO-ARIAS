#include "Game.h"

int main(int argc, char* argv[])
{
    Game* Juego;
    Juego = new Game(1024, 768, "Ragdoll Launcher - MAVI II - Emiliano Arias");
    Juego->Run();

    return 0;
}
