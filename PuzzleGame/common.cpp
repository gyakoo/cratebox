#include <common.h>

namespace PuzzleGame
{

Common::Common(uint32_t width, uint32_t height, const std::string& title, bool fullscreen )
  : m_width(width), m_height(height), m_sdlWindow(nullptr), m_sdlRenderer(nullptr)
{
    // System
    if ( SDL_Init(SDL_INIT_EVERYTHING) != 0 )
      throw std::exception("sdl init error");

    if ( TTF_Init() != 0 )
      throw std::exception("ttf init error");

    // Window
    uint32_t winFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;
    if ( fullscreen )
      winFlags |= SDL_WINDOW_FULLSCREEN;
    m_sdlWindow =  SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
      width, height, winFlags );
    if ( !m_sdlWindow )
      throw std::exception("error creating window");

    // Renderer
    uint32_t renderFlags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    m_sdlRenderer = SDL_CreateRenderer(m_sdlWindow, -1, renderFlags);
    if ( !m_sdlRenderer)
    {
      SDL_DestroyWindow(m_sdlWindow);
      throw std::exception("cannot create renderer");
    }

    SDL_RenderSetLogicalSize(m_sdlRenderer, width, height);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, 0);
    SDL_SetRenderDrawBlendMode(m_sdlRenderer, SDL_BLENDMODE_BLEND);
}

Common::~Common()
{
  SDL_DestroyWindow(m_sdlWindow);
  SDL_DestroyRenderer(m_sdlRenderer);
  TTF_Quit();
  SDL_Quit();
}

bool Common::BeginLoop()
{
  // clear
  SDL_SetRenderDrawColor( m_sdlRenderer, 40,40,40,255 );
  if ( SDL_RenderClear( m_sdlRenderer ) != 0 )
    return true;

  // events
  SDL_Event evt;
  while ( SDL_PollEvent(&evt) )
  {
    switch ( evt.type )
    {
    case SDL_QUIT: 
      return false;
    case SDL_KEYDOWN:
      std::for_each( m_keyListeners.begin(), m_keyListeners.end(),
        [evt](IKeyListener* l) 
      { 
        l->OnKeyDown(evt.key.keysym.scancode); 
      });
      break;
    case SDL_KEYUP:
      std::for_each( m_keyListeners.begin(), m_keyListeners.end(),
        [evt](IKeyListener* l) 
      { 
        l->OnKeyUp(evt.key.keysym.scancode); 
      });
      break;
    }
  }
  // draw
  return true;
}

void Common::EndLoop()
{
  SDL_RenderPresent(m_sdlRenderer);
}

void Common::FillRect( const Rect& rect, const Color& color )
{
  SDL_SetRenderDrawColor( m_sdlRenderer, color.r, color.g, color.b, color.a );
  SDL_RenderFillRect( m_sdlRenderer, (const SDL_Rect*)&rect );
}

uint32_t Common::GetWidth()
{
  return m_width;
}

uint32_t Common::GetHeight()
{
  return m_height;
}

void Common::AddKeyListener( IKeyListener* kl )
{
  m_keyListeners.push_back( kl );
}

void Common::RemoveKeyListener( IKeyListener* kl )
{
  std::remove_if( m_keyListeners.begin(), m_keyListeners.end(),
    [kl](IKeyListener* p){ return p==kl; } );
}

void Common::PostQuitEvent()
{
  SDL_Event evt;
  evt.type = SDL_QUIT;
  SDL_PushEvent(&evt);
}

uint32_t Common::GetTimerTicks()
{
  return SDL_GetTicks();
}

};