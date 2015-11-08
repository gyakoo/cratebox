#include <gamestates.h>
#include <stdarg.h>

namespace PuzzleGame
{

  GameStatePlaying::GameStatePlaying(std::shared_ptr<FSMManager> fsm, std::shared_ptr<Common> comm)
    : m_fsm(fsm), m_common(comm), m_timer(comm)
  {
  }

  void GameStatePlaying::OnEnter()
  {
    m_board = std::make_shared<Board>(m_common, BOARD_DIM_DEFAULT);
    m_player = std::unique_ptr<Player>(new Player(m_common, m_board));
    m_timer.AddCallback( std::chrono::milliseconds(1000), std::bind(&GameStatePlaying::OnAdvancePieces, this) );
    m_timer.AddCallback( std::chrono::milliseconds(2500), std::bind(&GameStatePlaying::OnCreatePiece, this) );
    m_font = std::make_shared<Font>(m_common, "OpenSans-Bold.ttf", 24);
    m_label = std::unique_ptr<Text>(new Text(m_font,"Hello world!",Colors::MSBLUE));
    OnCreatePiece();
    m_points = 0;
  }

  void GameStatePlaying::OnAdvancePieces()
  {
    std::for_each( m_pieces.begin(), m_pieces.end(), [this](Piece& p)
    {
      switch ( p.Advance(this->m_board) )
      {
        case PAR_HIT_OK   : this->HitOk(p); break;
        case PAR_HIT_WRONG: this->HitMiss(p); break;
      }
    });

    // remove all invalid pieces
    m_pieces.erase( std::remove_if( m_pieces.begin(), m_pieces.end(), [](Piece& p)
    {
      return p.invalid;
    }), m_pieces.end() );
  }

  void GameStatePlaying::HitOk(Piece& p)
  {
    m_player->HitOk(p.x,p.y);
    ++m_points;
    p.invalid=true;
    m_label->SetText( StringUtils::From(m_points) );
  }

  void GameStatePlaying::HitMiss(Piece& p)
  {
    m_player->HitWrong(p.x,p.y); 
    --m_points;
    p.invalid=true;
    m_label->SetText( StringUtils::From(m_points) );
  }

  void GameStatePlaying::OnCreatePiece()
  {
    // Pick random track
    int track = m_board->PickRandomTrack();
    if ( track >= 0 )
    {
      Piece p;
      m_board->GetTrackStart((uint32_t)track, p.x, p.y);
      m_board->GetTrackDir((uint32_t)track, p.dx, p.dy);
      p.invalid = false;
      m_pieces.push_back( p );
      auto& t=m_board->GetTile((uint32_t)p.x, (uint32_t)p.y);
      t.m_type = m_board->PickRandomTileType();
      t.m_status = Board::SMOVING;
    }
  }

  void GameStatePlaying::OnUpdate()
  {
    m_timer.Update();
    m_board->Draw();
    m_label->Render( 10, 10 );
  }

  void GameStatePlaying::OnExit()
  {
    m_player = nullptr;
    m_board = nullptr;
    m_timer.Reset();
  }

  void GameStatePlaying::OnKeyDown(int scancode)
  {
    switch (scancode)
    {
    case SDL_SCANCODE_LEFT : m_player->RotateLeft(); break;
    case SDL_SCANCODE_RIGHT: m_player->RotateRight(); break;
    case SDL_SCANCODE_ESCAPE: m_common->PostQuitEvent(); break;
    }
  }

  GameStatePlaying::PieceAdvanceResult GameStatePlaying::Piece::Advance(std::shared_ptr<Board> board)
  {
    auto& prev = board->GetTile(x,y);
    auto& next = board->GetTile(x+=dx, y+=dy);

    PieceAdvanceResult res=PAR_NONE;

    // is next position one logo tile? 
    if ( next.m_status == Board::SFIXED )
    {
      res = ( next.m_type == prev.m_type ) ? PAR_HIT_OK : PAR_HIT_WRONG; 
    }
    else
    {
      next = prev;
    }

    // invalidate last position
    prev.m_status= Board::SNONE;
    prev.m_type = Board::TNONE;
    return res;
  }

};