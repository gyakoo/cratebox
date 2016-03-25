//
//
#include <stdio.h>
#include <conio.h>
#include <Windows.h>
#include <hkvdb.hpp>

#define UB_IMPLEMENTATION
#include <ubody.hpp>

class ubDemo
{
public:
	ubDemo() : m_ubody(NULL)
		, m_world(UB_NULL_WORLD)
	{
	}

	~ubDemo()
	{
		quit();
	}

	int init()
	{
    int flags = 0;
		ub_create(&m_ubody, flags);
		ub_world_desc wdesc = { XMFLOAT3(0.0f, -9.81f, 0.0f) };
		m_world = ub_world_create(m_ubody, &wdesc);
	}

	void quit()
	{
		ub_destroy(&m_ubody);
	}

private:
	ubody* m_ubody;
	UB_WORLD m_world;
};

//
//
// Main console entry point
//
int main()
{
	// initialize Havok base and AI
	Renderer renderer;
	if ( renderer.init() != 0 )
		return 1;

	printf( "Press ESC on this console to exit.\n");

	// Main loop
	// main application loop
	const DWORD frameSleepMs = 10;
	const float aiStepSecs = 1.0f/60.0f;
	bool ended = false;
	while ( !ended )
	{
		renderer.step(aiStepSecs);
		Sleep( frameSleepMs );

		ended = _kbhit() &&  _getch() == 0x1b; // exit on ESCAPE
	}
	renderer.quit();
	return 1;
}
