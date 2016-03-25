//
//
#include <stdio.h>
#include <conio.h>
#include <Windows.h>

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
	printf( "Press ESC on this console to exit.\n");

	// Main loop
	// main application loop
	const DWORD frameSleepMs = 10;
	bool ended = false;
	while ( !ended )
	{
		Sleep( frameSleepMs );

		ended = _kbhit() &&  _getch() == 0x1b; // exit on ESCAPE
	}
	return 1;
}
