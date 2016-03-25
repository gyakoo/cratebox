#ifndef _FILE_H_
#define _FILE_H_

struct File;
struct gyFileResolver;
class gyEngine;
// ***********************************************************************************************
// Abstract filesystem singleton class
// ***********************************************************************************************
class gyFileSystem : public gyReflectedObject
{
  GY_DECLARE_REFLECT(gyFileSystem);
public:  
  enum eCreateOptions { OP_NONE=0, OP_READ=1, OP_WRITE=2 };   // file creation options
  enum eSeek          { SK_SET=0, SK_CUR, SK_END };           // seek positions
  enum                { MAX_SEARCH_PATHS=16, MAX_OPEN_FILES=256 }; // max number of handlers
  // holds a search path entry information
  struct SearchPathEntry
  { 
    SearchPathEntry():resolver(0){}
    bool operator ==(int nullval){ return nullval==0 && !resolver; } // equal == 0 overload to indicate is null
    
    gyFileResolver* resolver; // abstract file resolver
  };
  // holds a file handler entry
  struct FileEntry
  {
    FileEntry():spIndex(-1),file(0){}
    bool operator ==(int nullval){ return nullval==0 && !file && spIndex==-1; }
    
    int spIndex;  // search path index
    File* file;   // abstract file
  };

public:
  // reflected object
  gy_override int Create(gyVarDict& params);
  gy_override void FillCreateTemplate(gyVarDict& outTemplate);

  // specific methods
  void    CloseAllFiles(); // closes and releases all current open files.
  void    ClearAllSearchPaths();

  void    AddSearchPath( const char* spath, gyFileResolver* fileResolver=0 ); // Null will create a native filesystem
  void    SetWritingPath( const char* wpath );
  gyIDFile  FileOpen( const char* name, eCreateOptions op=OP_READ );
  void    FileClose( gyIDFile& fid );
  int     FindFile( const char* name );
  const char* GetSearchPath(int index);
  uint64_t  FileLength( gyIDFile fid );
  uint64_t  FileRead( gyIDFile fid, void* dstBuffer, uint64_t bytesToRead );
  uint64_t  FileWrite( gyIDFile fid, const void* srcBuffer, uint64_t bytesToWrite );
  uint64_t  FileTell( gyIDFile fid );
  void    FileSeek( gyIDFile fid, uint64_t offset, eSeek seekOption=SK_SET );
  const char* FileName( gyIDFile fid );

  // todo: async API here

private:
  friend class gyEngine;
  gyFileSystem();
  ~gyFileSystem();
  typedef std::vector<SearchPathEntry> SearchPathArray;
  typedef std::vector<FileEntry> FileArray;

  SearchPathArray searchPaths;
  FileArray       filesArray;
  SearchPathEntry writingPath;
};

// *************************************************************************************************
// *************************************************************************************************
class gyFileSystemHelper
{
public:
  // path helper methods
  static void NormalizePath( const char* path, char* dstPath, int maxSize, int pathLength );
  static const char* NormalizePath( std::string& outPath );
  static const char* ComposeSearchPathAndFileName( const char* filename );
  static const char* ComposeSearchPathAndFileName( int searchPath, const char* filename );

  static int64_t FileReadToEnd( const char* filename, void* dstBuffer, uint64_t maxBytes );
  static int64_t FileReadToEnd( gyIDFile fid, void* dstBuffer, uint64_t maxBytes );

  static bool FileExists( const char* filename );
};

// *************************************************************************************************
// *************************************************************************************************
struct gyFileResolver
{
  virtual ~gyFileResolver(){}
  virtual void Destroy() = 0;
  virtual bool FileExists( const char* file ) = 0;
  virtual File* FileOpen( const char* file, gyFileSystem::eCreateOptions op ) = 0;
  virtual const char* GetPath() = 0;
};

#endif