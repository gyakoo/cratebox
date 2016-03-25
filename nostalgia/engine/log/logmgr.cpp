#include <stdafx.h>
#include <engine/log/logmgr.h>

#include GY_PLATINC_LOGMGR

const char* gyLogManager::LogLevelToStr(gyLogLevel level)
{
  // heard you like funk?
  return "Error  \0Warning\0Info   \0Debug  " + (level<<3);
}

gyLogManager::gyLogManager()
{
}

gyLogManager::~gyLogManager()
{
  ReleaseAll();
}

int gyLogManager::Create(gyVarDict& createParams)
{
  bool createdDebug = false;
  if ( createParams.Has("loggers", gyVariant::T_VLIST ) )
  {
    gyVarList& loggers = createParams["loggers"].GetVL();
    for ( int i = 0; i < loggers.Size(); ++i )
    {
      if ( loggers.Get(i).Type() == gyVariant::T_VTABLE )
      {
        gyVarDict& logger = loggers.Get(i).GetVT();
        int64_t logType = logger.GetXPath("type",GY_LOGWRITER_TO_TEXT_FILE);
        if ( logType < GY_LOGWRITER_TO_TEXT_FILE || logType >= GY_LOGWRITER_MAXPRESET )
          logType = GY_LOGWRITER_TO_TEXT_FILE;
        WriterCreate( (gyLogWriterPresets)logType, logger );
        if (!createdDebug && logType == GY_LOGWRITER_TO_DEBUGCONSOLE ) 
          createdDebug = true;
      }
    }
  }
#ifdef BX_PLATFORM_WINDOWS
  if ( !createdDebug )
  {
    gyVarDict empty;
    WriterCreate( GY_LOGWRITER_TO_DEBUGCONSOLE, empty );
  }
#endif

  return R_OK;
}

void gyLogManager::FillCreateTemplate(gyVarDict& outTemplate)
{

}

// writers management
gyIDLogWriter gyLogManager::WriterCreate( gyLogWriterPresets type, const gyVarDict& params )
{
  gySharedPtr<gyLogWriter> writer;
  switch ( type )
  {
    case GY_LOGWRITER_TO_TEXT_FILE: 
    {
      const char* filename = params["filename"].GetS();
      if ( !filename ) filename = GY_ENGINE_FULLNAME".log";
      const char* fmt = params["format"].GetS();
      if ( !fmt ) fmt = "text";
      if ( gyStringUtil::StrCmp(fmt, "text") == 0 )
        writer = new gyLogWriterTextFile( filename ); 
      else if ( gyStringUtil::StrCmp(fmt, "html") == 0 )
        writer = new gyLogWriterHTMLFile( filename );
    }break;
    case GY_LOGWRITER_TO_BIN_FILE: 
      writer = new gyLogWriterBinFile( params["filename"].GetS() ); 
    break;
    case GY_LOGWRITER_TO_DEBUGCONSOLE : 
      writer = new gyLogWriterDebugConsole(); 
    break;
  }
  return writers.Add(writer);
}

gyIDLogWriter gyLogManager::WriterRegister( gyLogWriter* writer )
{
  FAIL_IF(!writer, "writer not valid");
  return writers.Add(writer);
}

gyLogWriter* gyLogManager::WriterGet(gyIDLogWriter idlw)
{
  return writers.Get(idlw);
}

void gyLogManager::WriterRelease(gyIDLogWriter& idLogw)
{
  writers.Release(idLogw);
}

void gyLogManager::ReleaseAll()
{
  writers.ReleaseAll();
}

// writing message to log
void gyLogManager::Message(gyLogLevel level, const char* format, ... )
{
  va_list arglist;
  va_start (arglist, format);
  vsnprintf_s(tmpMsg, sizeof(tmpMsg), format, arglist);
  va_end (arglist);

  int count = writers.GetSize();
  for ( int i = 0; i < count; ++i )
  {
    gyLogWriter* logw = writers.GetByNdx(i);
    if ( !logw || !logw->IsEnabled() ) continue;    
    if ( level <= logw->GetLevel() )
      logw->Write(level, tmpMsg);
  }
}

void gyLogManager::Message(gyIDLogWriter idlw, gyLogLevel level, const char* format, ... )
{
  va_list arglist;
  va_start (arglist, format);
  vsnprintf_s(tmpMsg, sizeof(tmpMsg), format, arglist);
  va_end (arglist);

  gyLogWriter* logw = writers.Get(idlw);
  if ( logw && logw->IsEnabled() && level <= logw->GetLevel() )
    logw->Write(level, tmpMsg);
}

void gyLogManager::Flush(gyIDLogWriter idlw)
{
  gyLogWriter* logw = writers.Get(idlw);
  if ( logw && logw->IsEnabled() )
    logw->Flush();
}

void gyLogManager::Flush()
{
  const int count = writers.GetSize();
  for ( int i = 0; i < count; ++i )
  {
    gyLogWriter* logw = writers.GetByNdx(i);
    if ( !logw || !logw->IsEnabled() ) continue;    
      logw->Flush();
  }
}

void gyLogManager::Clear(gyIDLogWriter idlw)
{
  gyLogWriter* logw = writers.Get(idlw);
  if ( logw && logw->IsEnabled() )
    logw->Clear();
}

void gyLogManager::Clear()
{
  const int count = writers.GetSize();
  for ( int i = 0; i < count; ++i )
  {
    gyLogWriter* logw = writers.GetByNdx(i);
    if ( !logw || !logw->IsEnabled() ) continue;    
    logw->Clear();
  }
}

// log level. If no id passed, will get the first one and set all of them.
gyLogLevel gyLogManager::GetLogLevel()
{
  const int count = writers.GetSize();
  for ( int i = 0; i < count; ++i )
  {
    gyLogWriter* logw = writers.GetByNdx(i);
    if ( logw )
      return logw->GetLevel();
  }
  return GY_LOG_UNKNOWN;
} 

gyLogLevel gyLogManager::GetLogLevel(gyIDLogWriter idlw)
{
  gyLogWriter* logw = writers.Get(idlw);
  return logw ? logw->GetLevel() : GY_LOG_UNKNOWN;
}

void gyLogManager::SetLogLevel(gyLogLevel level)
{
  const int count = writers.GetSize();
  for ( int i = 0; i < count; ++i )
  {
    gyLogWriter* logw = writers.GetByNdx(i);
    if ( logw )
      logw->SetLevel(level);
  }
}

void gyLogManager::SetLogLevel(gyIDLogWriter idlw, gyLogLevel level)
{
  gyLogWriter* logw = writers.Get(idlw);
  if ( logw )
    logw->SetLevel(level);
}
