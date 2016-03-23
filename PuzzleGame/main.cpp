#include <base/common.h>
#include <base/game.h>


using namespace Game;

int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
  auto engine = std::make_shared<Engine>( WIN_WIDTH_DEFAULT, WIN_HEIGHT_DEFAULT, "GameApp", false );
  GameApp game(engine);
  game.MainLoop();
  
  return S_OK;
}
