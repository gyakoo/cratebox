#include <fsm.h>

namespace PuzzleGame
{
  template<typename SHT, typename ET>
  FSMTransition<SHT,ET>::FSMTransition( SHT curSt, ET event, SHT nextSt )
    : m_currentState(curSt), m_event(event), m_nextState(nextSt)
  {
  }

  template<typename SHT, typename ET>
  bool FSMTransition<SHT,ET>::operator ==(const FSMTransition& o)
  {
    return m_event == o.m_event && m_currentState == o.m_currentState 
      && m_nextState == o.m_nextState;
  }

  template<typename SHT, typename ET>
  bool FSMTransition<SHT,ET>::Check(SHT curSt, ET evt, SHT& nextSt) const
  {
    bool valid = evt == m_event && curSt == m_currentState;
    if ( valid )
      nextSt = m_nextState;
    return valid;
  }

  FSMManager::FSMManager()
    : m_current( HINVALID )
  {
  }

  FSMManager::StateHandle FSMManager::AddState( std::shared_ptr<FSMState> state )
  {
    return AddElementTo<StateHandle,std::shared_ptr<FSMState>,StateList>(state,m_states);
  }

  FSMManager::TransHandle FSMManager::AddTransition( const TransitionType& trans )
  {
    return AddElementTo<TransHandle,TransitionType,TransitionList>(trans,m_transitions);
  }

  std::shared_ptr<FSMState> FSMManager::GetState( StateHandle hState )
  {
    return hState == HINVALID ? nullptr : m_states[hState];
  }

  FSMManager::StateHandle FSMManager::GetCurrentState( )
  {
    return m_current;
  }

  FSMManager::TransitionType& FSMManager::GetTransition( TransHandle hTrans )
  {
    if ( hTrans == HINVALID ) throw std::exception("invalid transition");
    return m_transitions[hTrans];
  }

  void FSMManager::SendEvent( EventType event )
  {
    // search the transition given the event and our current staet
    TransitionList::iterator it = m_transitions.begin();
    StateHandle nextSt;
    for ( ; it != m_transitions.end(); ++it )
    {
      if ( it->Check(m_current,event,nextSt) )
      {
        auto stCur = GetState( m_current );
        if ( stCur != nullptr )
          stCur->OnExit();
        m_current = HINVALID;

        auto stNext = GetState( nextSt );
        if ( stNext != nullptr )
        {
          stNext->OnEnter();
          m_current = nextSt;
        }
        break; 
      }
    }
  }

  void FSMManager::Start( StateHandle hState)
  {
    auto st = GetState(hState);
    if ( st != nullptr )
    {
      st->OnEnter();
      m_current = hState;
    }
  }

  void FSMManager::Update()
  {
    auto stCur = GetState( m_current );
    if ( stCur != nullptr )
    {
      stCur->OnUpdate();
    }
  }

  template<typename R, typename E, typename C>
  R FSMManager::AddElementTo(E e, C& container)
  {
    C::iterator it = std::find( container.begin(), container.end(), e );
    if ( it == container.end() )
    {
      container.push_back(e);
      return (int)container.size() - 1;
    }else
    {
      return (int)(it - container.begin());
    }
  }

};