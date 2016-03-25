#include <d3d9.h> // have to use legacy dx9 headers for d3dperf
#include <Windows.h>
#include <stdio.h>

//////////////////////////////////////////////////////////////////////////
void gyAssert( bool cond, const char* file, int line, const char* condstr, const char* msg )
{
  if ( cond ) return;
  char assmsg[512];
  sprintf_s( assmsg, "Location: %s(%d)\nCondition Failed: %s\nMessage: \"%s\"\nContinue?",
    file,line,condstr,msg);
  int res = ::MessageBoxA( NULL, assmsg, "Assertion failed!", MB_YESNO|MB_ICONERROR|MB_TOPMOST);
  if ( res == IDNO )
  {
    //__asm int 3;
    __debugbreak();
  }
}

#ifdef _DEBUG
//////////////////////////////////////////////////////////////////////////
void gyPerfMarkerBegin( const char* name )
{
  wchar_t szMarkerString[256];
  int len = MultiByteToWideChar(CP_ACP, 0, name, -1, NULL, 0);
  MultiByteToWideChar(CP_ACP, 0, name, -1, szMarkerString, len);
  D3DPERF_BeginEvent(0x0000FF00, szMarkerString);
}
void gyPerfMarkerEnd()
{
  D3DPERF_EndEvent();
}
#else
void gyPerfMarkerBegin( const char* ){ }
void gyPerfMarkerEnd(){ }
#endif

//////////////////////////////////////////////////////////////////////////
bool gyWinIsVersionOrGreater(uint16_t major, uint16_t minor, uint16_t spmajor)
{
  OSVERSIONINFOEXW osvi = { sizeof(OSVERSIONINFOEXW), 0, 0, 0, 0, {0}, 0, 0 };
  DWORDLONG        const dwlConditionMask = VerSetConditionMask(
    VerSetConditionMask(
    VerSetConditionMask(
    0, VER_MAJORVERSION, VER_GREATER_EQUAL),
    VER_MINORVERSION, VER_GREATER_EQUAL),
    VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);

  osvi.dwMajorVersion = major;
  osvi.dwMinorVersion = minor;
  osvi.wServicePackMajor = spmajor;

  return VerifyVersionInfoW(&osvi, VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR, dwlConditionMask) != FALSE;
}

bool gyWinIsVistaOrGreater()
{
  return gyWinIsVersionOrGreater(HIBYTE(_WIN32_WINNT_VISTA), LOBYTE(_WIN32_WINNT_VISTA), 0);
}

bool gyWinIs7SP1OrGreater()
{
  return gyWinIsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN7), LOBYTE(_WIN32_WINNT_WIN7), 1);
}

//////////////////////////////////////////////////////////////////////////
// http://alter.org.ua/docs/win/args/
PCHAR* CommandLineToArgvA(PCHAR CmdLine,int* _argc)
{
  PCHAR* argv;
  PCHAR  _argv;
  ULONG   len;
  ULONG   argc;
  CHAR   a;
  ULONG   i, j;

  BOOLEAN  in_QM;
  BOOLEAN  in_TEXT;
  BOOLEAN  in_SPACE;

  len = (ULONG)strlen(CmdLine);
  i = ((len+2)/2)*sizeof(PVOID) + sizeof(PVOID);

  argv = (PCHAR*)GlobalAlloc(GMEM_FIXED,
    i + (len+2)*sizeof(CHAR));

  _argv = (PCHAR)(((PUCHAR)argv)+i);

  argc = 0;
  argv[argc] = _argv;
  in_QM = FALSE;
  in_TEXT = FALSE;
  in_SPACE = TRUE;
  i = 0;
  j = 0;

  while( a = CmdLine[i] ) {
    if(in_QM) {
      if(a == '\"') {
        in_QM = FALSE;
      } else {
        _argv[j] = a;
        j++;
      }
    } else {
      switch(a) {
      case '\"':
        in_QM = TRUE;
        in_TEXT = TRUE;
        if(in_SPACE) {
          argv[argc] = _argv+j;
          argc++;
        }
        in_SPACE = FALSE;
        break;
      case ' ':
      case '\t':
      case '\n':
      case '\r':
        if(in_TEXT) {
          _argv[j] = '\0';
          j++;
        }
        in_TEXT = FALSE;
        in_SPACE = TRUE;
        break;
      default:
        in_TEXT = TRUE;
        if(in_SPACE) {
          argv[argc] = _argv+j;
          argc++;
        }
        _argv[j] = a;
        j++;
        in_SPACE = FALSE;
        break;
      }
    }
    i++;
  }
  _argv[j] = '\0';
  argv[argc] = NULL;

  (*_argc) = argc;
  return argv;
}

static int g_commandLineCount=0;
static char** g_commandLine=NULL;
int gyGetCommandLine(char**& argv)
{
  if ( g_commandLine )
  {
    argv = g_commandLine;
    return g_commandLineCount;
  }

  LPSTR cmdLine = ::GetCommandLineA();
  g_commandLine = CommandLineToArgvA(cmdLine,&g_commandLineCount);
  argv = g_commandLine;
  return g_commandLineCount;
}

void gyFreeCommandLine()
{
  if ( !g_commandLine )
    return;
  GlobalFree( (HGLOBAL)g_commandLine );
  g_commandLine = NULL;
  g_commandLineCount = 0;
}

// ensure it gets freed
struct cmdDeallocator
{
  ~cmdDeallocator(){ gyFreeCommandLine();}
}cmdDealloc;
