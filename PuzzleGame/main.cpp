#include <common.h>
#include <game.h>

int CALLBACK _main(int , const char**)
{
  return 0;
}

int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
  using namespace PuzzleGame;
  auto engine = std::make_shared<Engine>( WIN_WIDTH_DEFAULT, WIN_HEIGHT_DEFAULT, "Puzzle Game", false );
  Game game(engine);
  game.MainLoop();
  
  return S_OK;
}
