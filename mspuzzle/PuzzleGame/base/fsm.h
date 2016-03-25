#pragma once
#include <base/common.h>

namespace Game
{
  // -------------------------------------
  class FSMState : public IKeyListener
  {
  public:
    virtual void OnEnter(){}
    virtual void OnUpdate(){}
    virtual void OnExit(){}
  };

  // -------------------------------------
  template<typename SHT, typename ET>
  struct FSMTransition
  {
    FSMTransition( SHT curSt=-1, ET event=-1, SHT nextSt=-1 );
    bool operator ==(const FSMTransition& o);
    bool Check(SHT curSt, ET evt, SHT& nextSt) const;

    SHT m_currentState; 
    ET m_event;         
    SHT m_nextState;
  };

  // -------------------------------------
  class FSMManager
  {
  public:  
    typedef int EventType;
    typedef int TransHandle;
    typedef int StateHandle;
    typedef std::vector<std::shared_ptr<FSMState>> StateList;
    typedef FSMTransition<StateHandle,EventType> TransitionType;
    typedef std::vector<TransitionType> TransitionList;
    enum { HINVALID=-1 };
  public:
    FSMManager();

    StateHandle AddState( std::shared_ptr<FSMState> state );
    TransHandle AddTransition( const TransitionType& trans );

    std::shared_ptr<FSMState> GetState( StateHandle hState );
    StateHandle GetCurrentState( );
    TransitionType& GetTransition( TransHandle hTrans );
    void SendEvent( EventType event );
    void Start( StateHandle hState);
    void Update();

  private:
    template<typename R, typename E, typename C>
    R AddElementTo(E e, C& container);

  private:
    StateList m_states;
    TransitionList m_transitions;
    StateHandle m_current;
  };
};