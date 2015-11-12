#pragma once

#include <Windows.h>
#include <vector>
#include <algorithm>
#include <string>
#include <map>
#include <array>
#include <chrono>
#include <functional>
#include <memory>
#include <random>

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
    BOARD_DIM_DEFAULT = 6,
    BOARD_MOVES_DEFAULT = 3,
    TILE_LIFE_SEC_DEFAULT = 6
  };

  class StringUtils
  {
  public:
    static std::string From(int i);
    static std::string From(float f);
    static std::string Format(const char* format, ...);
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

    void Translate(int dx, int dy)
    {
      x+=dx;
      y+=dy;
    }

    void Deflate(int nx, int ny)
    {
      const auto hx = nx/2;
      const auto hy = ny/2;
      x += hx;
      y += hy;
      width -= nx;
      height -= ny;
    }
  };

  struct Color
  {
    uint8_t r,g,b,a;

    Color(){}
    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
      : r(r), g(g), b(b), a(a)
    {}
    bool operator ==(const Color& rhs){ return r==rhs.r && g==rhs.g && b==rhs.b && a==rhs.a; }
    bool operator !=(const Color& rhs){ return !operator ==(rhs); }
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

    static const Color GRAYWORKSPACE=Color(40,40,40,255);
  };

  class IKeyListener
  {
  public:
    IKeyListener(){}
    virtual ~IKeyListener(){}
    virtual void OnKeyDown(int scancode) { UNREFERENCED_PARAMETER(scancode); };
    virtual void OnKeyUp(int scancode) { UNREFERENCED_PARAMETER(scancode); };
  };

  class Font;
  class Text;

  class Engine
  {
  public:  
    Engine( uint32_t width, uint32_t height, const std::string& title, bool fullscreen );
    ~Engine();
    bool BeginLoop();
    void EndLoop();

    void FillRect( const Rect& rect, const Color& color );
    void DrawRect( const Rect& rect, const Color& color );
    void DrawLine( uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, const Color& color );

    uint32_t GetWidth();
    uint32_t GetHeight();
    void AddKeyListener(IKeyListener* kl );
    void RemoveKeyListener( IKeyListener* kl );
    void PostQuitEvent();
    std::chrono::milliseconds GetTimerTicks();
    std::chrono::milliseconds GetTimerDelta();

    SDL_Renderer* GetRenderer(){ return m_sdlRenderer; }
    SDL_Window* GetWindow(){ return m_sdlWindow; }
    std::shared_ptr<Font> GetFont(const std::string& fontName, uint32_t size);
  
  protected:
    typedef std::pair<std::string, uint32_t> FontNameSize;

    SDL_Window* m_sdlWindow;
    SDL_Renderer* m_sdlRenderer;    
    uint32_t m_width, m_height;
    std::chrono::milliseconds m_lastTicks;
    std::vector< IKeyListener* > m_keyListeners; // todo: change to use shared_ptr but gives me destruction issues for Player deriving from IKeyListener
    std::map< FontNameSize, std::shared_ptr<Font> > m_fonts;
  };

  class Font
  {
  public:
    Font( std::shared_ptr<Engine> engine, const std::string& fontName, int fontSize );
    ~Font();
    void SetSize(int fontSize);

  private:
    friend class Text;
    void _CreateFont();

    std::shared_ptr<Engine> m_engine;
    TTF_Font* m_ttfFont;
    std::string m_fontName;
    int m_fontSize;
  };

  class Text
  {
  public:
    Text(std::shared_ptr<Font> font
      , const std::string& text="", const Color& color=Colors::WHITE);
    ~Text();

    void SetText(const std::string& text);
    void SetColor(const Color& color);
    void Draw(const Rect& rect);
    void Draw(int x, int y);

  private:
    friend class Engine;
    void _CreateText();
    void _DestroyText();

    SDL_Texture* m_textTexture;
    std::shared_ptr<Font> m_font;
    std::string m_text;
    int m_width;
    int m_height;
    Color m_color;
  };

}; // ns