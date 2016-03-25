  #include <gamePCH.h>

int RunTest(int64_t test);
int PlaySingleLevel(const char* level);
int RunTexTool(int64_t port);
int RunLevelEditor(int64_t port);

MAIN_FUNCTION
{
  char** argv=NULL;
  int argc= gyGetCommandLine(argv);
  for ( int i = 0; i < argc; ++i )
  {
    if ( (i+1)<argc && gyStringUtil::StrCmp(argv[i], "-playlevel") == 0 )
    {
      if ( PlaySingleLevel(argv[i+1]) != R_OK )
        return -1;
    }
    else if ( (i+1)<argc && gyStringUtil::StrCmp(argv[i], "-textool") == 0 )
    {
      int64_t port = gyStringUtil::ParseNumber(argv[i+1]);
      if ( RunTexTool(port) != R_OK )
        return -1;
    }
    else if ((i+1)<argc && gyStringUtil::StrCmp(argv[i], "-leveled") == 0 )
    {
      int64_t port = gyStringUtil::ParseNumber(argv[i+1]);
      if ( RunLevelEditor(port) != R_OK )
        return -1;
    }
    else if ( (i+1)<argc && gyStringUtil::StrCmp(argv[i], "-runtest") == 0 )
    {
      int64_t test = gyStringUtil::ParseNumber(argv[i+1]);
      if ( RunTest(test) != R_OK )
        return -1;
    }
  }
  return R_OK;
}