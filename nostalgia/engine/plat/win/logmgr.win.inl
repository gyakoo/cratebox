
//////////////////////////////////////////////////////////////////////////
class gyLogWriterTextFile : public gyLogWriter
{
public:
  gyLogWriterTextFile(const char* filename);
  ~gyLogWriterTextFile();
  gy_override void Write( gyLogLevel level, const char* msg ) ;
  gy_override void Flush();
  gy_override void Clear();
  gy_override void SetEnabled( bool status ){ enabled=status; }
  gy_override void SetLevel( gyLogLevel level ){ this->level = level; }
  gy_override gyLogLevel GetLevel(){ return level; }
  gy_override bool IsEnabled(){ return enabled; }

  HANDLE hfile;
  gyLogLevel level;
  bool enabled;
};


//////////////////////////////////////////////////////////////////////////
class gyLogWriterHTMLFile : public gyLogWriter
{
public:
  gyLogWriterHTMLFile(const char* filename){}
  ~gyLogWriterHTMLFile(){}
  gy_override void Write( gyLogLevel level, const char* msg ){}
  gy_override void Flush(){}
  gy_override void Clear(){}
  gy_override void SetEnabled( bool status ){}
  gy_override void SetLevel( gyLogLevel level ){}
  gy_override gyLogLevel GetLevel(){return GY_LOG_UNKNOWN; }
  gy_override bool IsEnabled(){return false;}
};

//////////////////////////////////////////////////////////////////////////
class gyLogWriterBinFile : public gyLogWriter
{
public:
  gyLogWriterBinFile(const char* filename){}
  gy_override void Write( gyLogLevel level, const char* msg ){}
  gy_override void Flush(){}
  gy_override void Clear(){}
  gy_override void SetEnabled( bool status ){}
  gy_override void SetLevel( gyLogLevel level ){}
  gy_override gyLogLevel GetLevel(){return GY_LOG_UNKNOWN; }
  gy_override bool IsEnabled(){return false;}
};

//////////////////////////////////////////////////////////////////////////
class gyLogWriterDebugConsole: public gyLogWriter
{
public:
  gyLogWriterDebugConsole():level(GY_LOG_DEBUG){}
  gy_override void Write( gyLogLevel level, const char* msg )
  {
    OutputDebugStringA( gyStrFormat256("%s: %s\n", gyLogManager::LogLevelToStr(level), msg) );
  }
  gy_override void Flush(){}
  gy_override void Clear(){}
  gy_override void SetEnabled( bool status ){ enabled=status; }
  gy_override void SetLevel( gyLogLevel level ){ this->level = level; }
  gy_override gyLogLevel GetLevel(){ return level; }
  gy_override bool IsEnabled(){ return enabled; }

  gyLogLevel level;
  bool enabled;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
OVERLAPPED g_overlapped={0};
gyLogWriterTextFile::gyLogWriterTextFile(const char* filename)
  : hfile(INVALID_HANDLE_VALUE), enabled(true), level(GY_LOG_DEBUG)
{
  // If this is the first call, then it succeeds to delete the file.
  // Otherwise, if this is the second call, it cannot delete because of the SHARE policy here, just append to the end.
  BOOL res = ::DeleteFileA( filename );

  // tries to create the file
  hfile = CreateFileA( filename,
                      GENERIC_WRITE,      // open for writing
                      FILE_SHARE_WRITE|FILE_SHARE_READ,   // share only for writing and reading
                      NULL,               // default security
                      OPEN_ALWAYS,
                      FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED, 
                      NULL );

  FAIL_IF( hfile == INVALID_HANDLE_VALUE, "Error creating overlapped file" );
  if ( hfile )
  {
    g_overlapped.Offset = g_overlapped.OffsetHigh = 0xffffffff;
  }
}

gyLogWriterTextFile::~gyLogWriterTextFile()
{
  if ( hfile != INVALID_HANDLE_VALUE )
  {
    Flush();
    CloseHandle(hfile);
  }
}

static const int IOMaxCharBuffers = 4;
static char IOCharBuffers[IOMaxCharBuffers][512] = {0};
static int IONextCharBuffer = 0;

void gyLogWriterTextFile::Write( gyLogLevel level, const char* msg )
{
  if ( hfile == INVALID_HANDLE_VALUE ) 
    return;
  int ndx = (IONextCharBuffer++)%IOMaxCharBuffers;
  sprintf_s(IOCharBuffers[ndx],"%s: %s\n", gyLogManager::LogLevelToStr(level), msg );
  DWORD bytesToWrite=gyStringUtil::StrLen(IOCharBuffers[ndx]);
  DWORD bytesWritten=0;
  // returns immediately, and the OS makes the write in parallel at the end of file
  BOOL res = WriteFile(hfile, IOCharBuffers[ndx], bytesToWrite, &bytesWritten, &g_overlapped);
  // can return false and error_io_pending to indicate the writting is being performed in background
  bool wasError = (!res && ::GetLastError() != ERROR_IO_PENDING) || (res && bytesWritten != bytesToWrite );
}

void gyLogWriterTextFile::Flush()
{
  if ( hfile != INVALID_HANDLE_VALUE )
    FlushFileBuffers(hfile);
}

void gyLogWriterTextFile::Clear()
{
}

/*
  // writing to file
  // returns immediately, and the OS makes the write in parallel at the end of file
  DWORD bytesWritten=0;
  BOOL res = WriteFile(m_hfile, tmpStr, bytesToWrite, &bytesWritten, &g_cigiOverlapped );  
  // can return false and error_io_pending to indicate the writting is being performed in background
  bool wasError = (!res && ::GetLastError() != ERROR_IO_PENDING) || (res && bytesWritten != bytesToWrite );
  VASSERT_MSG( !wasError, "Error writing log message");

  // enqueing for visual vision print
  if ( m_visual )
  {
    m_mutex.Lock();
    strcpy_s(m_messages[m_cursor], tmpStr );
    m_cursor = (m_cursor+1) % m_maxMsgs;
    m_mutex.Unlock();
  }
}
*/