#include <common.h>
#include <game.h>

int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
  using namespace PuzzleGame;
  auto comm = std::make_shared<Common>( 1024, 768, "Puzzle Game", false );
  Game game(comm);
  game.MainLoop();
  
  return S_OK;
}
