// gentex.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "gentex.h"


// This is an example of an exported variable
GENTEX_API int ngentex=0;

// This is an example of an exported function.
GENTEX_API int fngentex(void)
{
	return 42;
}


GENTEX_API int filterNumber(float num)
{
  return *((int*)&num);
}

// This is the constructor of a class that has been exported.
// see gentex.h for the class definition
Cgentex::Cgentex()
{
	return;
}

GENTEX_API float doSomething(int a, int b, const char* algo)
{
  return float(a)/b + strlen(algo);
}
