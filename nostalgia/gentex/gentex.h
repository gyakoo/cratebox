// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the GENTEX_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// GENTEX_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef GENTEX_EXPORTS
#define GENTEX_API __declspec(dllexport)
#else
#define GENTEX_API __declspec(dllimport)
#endif

// This class is exported from the gentex.dll
class GENTEX_API Cgentex {
public:
	Cgentex(void);
	// TODO: add your methods here.
};

extern "C"
{
  extern GENTEX_API int ngentex;

  GENTEX_API int fngentex(void);
  GENTEX_API int filterNumber(float num);
  GENTEX_API float doSomething(int a, int b, const char* algo);
};