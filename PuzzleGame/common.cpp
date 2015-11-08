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

Font::Font( std::shared_ptr<Common> comm, const std::string& fontName, int fontSize )
  : m_common(comm), m_fontName(fontName), m_fontSize(fontSize)
{
  _CreateFont();
}

Font::~Font()
{
  TTF_CloseFont(m_ttfFont);
}

void Font::SetSize(int fontSize)
{
  if ( fontSize != m_fontSize )
  {
    m_fontSize = fontSize;
    _CreateFont();
  }
}

void Font::_CreateFont()
{
  m_ttfFont = TTF_OpenFont(m_fontName.c_str(), m_fontSize);
  if ( !m_ttfFont )
    throw std::exception("Cannot create font");
}

Text::Text(std::shared_ptr<Font> font,const std::string& text, const Color& color)
  : m_text(text), m_color(color), m_textTexture(nullptr), m_font(font)
{
  _CreateText();
}

Text::~Text()
{
  _DestroyText();
}

void Text::SetText(const std::string& text)
{
  if ( m_text != text )
  {
    m_text = text;
    _CreateText();
  }
}
void Text::SetColor(const Color& color)
{
  if ( m_color != color )
  {
    m_color = color;
    _CreateText();
  }
}

void Text::_CreateText()
{
  if ( m_text.empty() )
    return;
  _DestroyText();
  SDL_Surface* tmpSurf = TTF_RenderText_Solid(m_font->m_ttfFont, m_text.c_str(), 
    *reinterpret_cast<SDL_Color*>(&m_color));
  if ( !tmpSurf )
    throw std::exception( "cannot create surface for text" );
  m_width = tmpSurf->w;
  m_height = tmpSurf->h;
  m_textTexture = SDL_CreateTextureFromSurface( m_font->m_common->GetRenderer(), tmpSurf );
  if ( !m_textTexture )
    throw std::exception( "cannot create text texture" );

  SDL_FreeSurface(tmpSurf);
}

void Text::_DestroyText()
{
  if ( m_textTexture )
  {
    SDL_DestroyTexture(m_textTexture);
    m_textTexture = nullptr;
  }
}

void Text::Render(const Rect& rect)
{
  SDL_RenderCopy(m_font->m_common->GetRenderer(), m_textTexture,
    nullptr, reinterpret_cast<const SDL_Rect*>(&rect));
}

void Text::Render(int x, int y)
{
  Rect r(x, y, m_width, m_height);
  Render(r);
}

  std::string StringUtils::From(int i)
  {
    std::array<char,512> s;
    _itoa_s(i,s.data(),s.size(),10);
    return std::string(s.data());
  }
  
  std::string StringUtils::From(float f)
  {
    std::array<char,512> s;
    sprintf_s(s.data(), s.size(),"%g",f);
    return std::string(s.data());
  }

  std::string StringUtils::Format(const char* format, ...)
  {
    std::array<char,512> s;
    va_list args;
    va_start( args, format );
    vsprintf_s(s.data(),s.size(), format, args );
    va_end( args );
    return std::string(s.data());
  }
};