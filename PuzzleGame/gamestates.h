#pragma once
#include <common.h>
#include <game.h>
#include <fsm.h>

namespace PuzzleGame
{
  enum GameEvents
  {
    GAMEVENT_GO_PLAY
  };

  class GameStatePlaying : public FSMState
  {
  public:
    GameStatePlaying(std::shared_ptr<FSMManager> fsm, std::shared_ptr<Common> comm);
    virtual void OnEnter();
    virtual void OnUpdate();
    virtual void OnExit();
    virtual void OnKeyDown(int scancode);

  private:
    enum PieceAdvanceResult { PAR_NONE, PAR_HIT_OK, PAR_HIT_WRONG };
    struct Piece
    {
      PieceAdvanceResult Advance(std::shared_ptr<Board> board);

      int x;
      int y;
      int dx;
      int dy;
      bool invalid;
    };

    void OnAdvancePieces();
    void OnCreatePiece();
    void HitOk(Piece& p);
    void HitMiss(Piece& p);

  private:
    GameTimer m_timer;
    std::shared_ptr<FSMManager> m_fsm;
    std::shared_ptr<Common> m_common;
    std::shared_ptr<Board> m_board;
    std::unique_ptr<Player> m_player;
    std::vector<Piece> m_pieces;
    std::shared_ptr<Font> m_font;
    std::unique_ptr<Text> m_label;
    int m_points;
  };

};