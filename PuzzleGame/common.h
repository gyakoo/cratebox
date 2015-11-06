#pragma once

#include <Windows.h>
#include <vector>
#include <algorithm>
#include <string>
#include <array>
#include <chrono>
#include <functional>
#include <memory>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <exception>
#include <stdint.h>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_keyboard.h>


namespace PuzzleGame
{

  enum PuzzleGameConstants
  {
    LOGO_N = 4,
    BOARD_DIM_DEFAULT = 14
  };

  struct Rect
  {
    int x, y;
    int width, height;

    Rect()
    {}
  
    Rect(int x, int y, int w, int h)
      : x(x), y(y), width(w), height(h)
    {}

    void Deflate(int x, int y)
    {
      const auto hx = x/2;
      const auto hy = y/2;
      this->x += hx;
      this->y += hy;
      width -= hx;
      height -= hy;
    }
  };

  struct Color
  {
    uint8_t r,g,b,a;

    Color(){}
    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
      : r(r), g(g), b(b), a(a)
    {}
  };

  namespace Colors
  {
    static const Color RED=Color(255,0,0,255);
    static const Color GREEN=Color(0,255,0,255);
    static const Color BLUE=Color(0,0,255,255);
    static const Color WHITE=Color(255,255,255,255);
    static const Color BLACK=Color(0,0,0,255);
    static const Color YELLOW=Color(255,255,0,255);

    static const Color MSRED=Color(242,80,34,255);
    static const Color MSGREEN=Color(127,186,0,255);
    static const Color MSBLUE=Color(0,164,239,255);
    static const Color MSYELLOW=Color(255,185,0,255);  
  };

  class IKeyListener
  {
  public:
    IKeyListener(){}
    virtual ~IKeyListener(){}
    virtual void OnKeyDown(int scancode) {};
    virtual void OnKeyUp(int scancode) {};
  };

  class Common
  {
  public:  
    Common( uint32_t width, uint32_t height, const std::string& title, bool fullscreen );
    ~Common();
    bool BeginLoop();
    void EndLoop();

    void FillRect( const Rect& rect, const Color& color );

    uint32_t GetWidth();
    uint32_t GetHeight();
    void AddKeyListener(IKeyListener* kl );
    void RemoveKeyListener( IKeyListener* kl );
    void PostQuitEvent();
    uint32_t GetTimerTicks();
  protected:
    SDL_Window* m_sdlWindow;
    SDL_Renderer* m_sdlRenderer;
    uint32_t m_width, m_height;
    std::vector< IKeyListener* > m_keyListeners; // todo: change to use shared_ptr but gives me destruction issues for Player deriving from IKeyListener
  };

}; // ns