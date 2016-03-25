#include <states/gamestates.h>
#include <stdarg.h>

namespace Game
{
    static uint32_t GROUND_HEIGHT = 200;
    static uint32_t BALL_HEIGHT = 300;

    GameStatePlaying::GameStatePlaying(std::shared_ptr<FSMManager> fsm, std::shared_ptr<Engine> engine)
        : m_fsm(fsm), m_engine(engine), m_timer(engine)
    {
    }

    void GameStatePlaying::OnEnter()
    {
        b2Vec2 gravity(0, -500);
        m_physics = std::make_unique<b2World>(gravity);
        
        {
            b2BodyDef groundBodyDef;
            b2Body* ground = m_physics->CreateBody(&groundBodyDef);
            b2EdgeShape shape;
            shape.Set(b2Vec2(-1000, GROUND_HEIGHT), b2Vec2(1000, GROUND_HEIGHT));
            ground->CreateFixture(&shape, 0.0f);
        }

        for (size_t i = 0; i < m_balls.size(); ++i )
        {
            b2BodyDef bd;
            bd.type = b2_dynamicBody;
            bd.position.Set(100 + i*8.0f, BALL_HEIGHT+14.0f*i);
            m_balls[i] = m_physics->CreateBody(&bd);
            b2CircleShape shape;
            shape.m_radius = 5.0f;
            m_balls[i]->CreateFixture(&shape, 10.0f);
        }
    }

    void GameStatePlaying::OnUpdate()
    {
        m_physics->Step(1.0f/60.0f, 6, 2);

        uint32_t h = m_engine->GetHeight()-  GROUND_HEIGHT+1;
        m_engine->DrawLine(0, h, m_engine->GetWidth(), h, Colors::WHITE);

        for (size_t i = 0; i < m_balls.size(); ++i)
        {
            const auto& p = m_balls[i]->GetPosition();
            h = m_engine->GetHeight() - (uint32_t)p.y;
            m_engine->DrawCircle((uint32_t)p.x, h, 5, Colors::MSRED);
        }

        //        uint32_t pheight = 18;
        //         Rect r(100, ground-pheight, pheight/3, pheight);
//         m_engine->FillRect(r, Colors::MSBLUE);

        m_timer.Update();
    }

    void GameStatePlaying::OnExit()
    {
        m_timer.Reset();
        m_physics.reset();
    }

    void GameStatePlaying::OnKeyDown(int scancode)
    {
        switch (scancode)
        {
        case SDL_SCANCODE_LEFT: break;
        case SDL_SCANCODE_RIGHT: break;
        case SDL_SCANCODE_ESCAPE: m_engine->PostQuitEvent(); break;
        }
    }

};