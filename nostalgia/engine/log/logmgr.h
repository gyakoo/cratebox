#ifndef _GY_LOGMGR_H_
#define _GY_LOGMGR_H_

class gyEngine;

enum gyLogLevel
{
  GY_LOG_UNKNOWN=-1,
  GY_LOG_ERROR = 0,
  GY_LOG_WARNING,
  GY_LOG_INFO,
  GY_LOG_DEBUG
};

enum gyLogWriterPresets
{
  GY_LOGWRITER_TO_TEXT_FILE=0,
  GY_LOGWRITER_TO_DEBUGCONSOLE=1,
  GY_LOGWRITER_TO_BIN_FILE=2,
  GY_LOGWRITER_MAXPRESET
};

class gyLogWriter : public gyResourceBase
{
public:
  virtual void Write( gyLogLevel level, const char* msg ) = 0;
  virtual void Flush() = 0;
  virtual void Clear() = 0;
  virtual void SetEnabled(bool status) = 0;
  virtual void SetLevel(gyLogLevel level) = 0;
  
  virtual gyLogLevel GetLevel() = 0;
  virtual bool IsEnabled() = 0;
};

class gyLogManager : public gyReflectedObject
{
  GY_DECLARE_REFLECT(gyLogManager);

public:
  gy_override void FillCreateTemplate(gyVarDict& outTemplate);
  gy_override int Create(gyVarDict& createParams);

  // writers management
  gyIDLogWriter WriterCreate( gyLogWriterPresets type, const gyVarDict& params );
  gyIDLogWriter WriterRegister( gyLogWriter* writer );
  gyLogWriter* WriterGet(gyIDLogWriter idlw);
  void WriterRelease(gyIDLogWriter& idLogw);
  void ReleaseAll();

  // writing message to log
  void Message(gyLogLevel level, const char* format, ... );
  void Message(gyIDLogWriter idlw, gyLogLevel level, const char* format, ... );
  void Flush(gyIDLogWriter idlw);
  void Flush();
  void Clear(gyIDLogWriter idlw);
  void Clear();

  // log level. If no id passed, will get the first one and set all of them.
  gyLogLevel GetLogLevel();
  gyLogLevel GetLogLevel(gyIDLogWriter idlw);
  void SetLogLevel(gyLogLevel level);
  void SetLogLevel(gyIDLogWriter idlw, gyLogLevel level);
  static const char* gyLogManager::LogLevelToStr(gyLogLevel level);

private:
  friend class gyEngine;
  gyLogManager();
  ~gyLogManager();

  char tmpMsg[2048]; 
  gyResourceMgr<gyLogWriter, ID_LOGWRITER> writers;
};


#endif