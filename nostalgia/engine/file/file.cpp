#include <stdafx.h>
#include <stdio.h>
#include <engine/file/file.h>

#define ROOT_FOLDER "./"
#define WRITING_FILE_PATH_INDEX -2

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------
struct File
{
  virtual ~File(){}
  virtual void Destroy() = 0;
  virtual uint64_t Tell() = 0;
  virtual void Seek( uint64_t offset, gyFileSystem::eSeek so ) = 0;
  virtual const char* GetFileName() = 0;
  virtual uint64_t Read( void* buf, uint64_t bytes ) = 0;
  virtual uint64_t Write( const void* buf, uint64_t bytes ) = 0;
  virtual gyFileSystem::eCreateOptions GetCreateOptions() = 0;
};

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------
struct FileNative : public File
{
  FileNative(const char* filename, gyFileSystem::eCreateOptions op );
  gy_override void Destroy();
  gy_override uint64_t Tell();
  gy_override void Seek( uint64_t offset, gyFileSystem::eSeek so );
  gy_override uint64_t Read( void* buf, uint64_t bytes );
  gy_override uint64_t Write( const void* buf, uint64_t bytes );
  gy_override gyFileSystem::eCreateOptions GetCreateOptions(){ return createOps; }

#ifdef _DEBUG
  gy_override const char* GetFileName(){ return fileName.c_str(); }
  std::string fileName;
#else
  gy_override const char* GetFileName(){return "";}
#endif
#if defined(BX_PLATFORM_WINDOWS)
  FILE* file;
#endif
  gyFileSystem::eCreateOptions createOps;
};

// ---
struct FileResolverNative : public gyFileResolver
{
  FileResolverNative( const char* path );

  gy_override void Destroy();
  gy_override bool FileExists( const char* file );
  gy_override File* FileOpen( const char* file, gyFileSystem::eCreateOptions op );
  gy_override const char* GetPath(){ return path.c_str(); }
  void SetNewPath( const char* newpath ){ path = newpath; }
  
  std::string path;
};

#include GY_PLATINC_FILE

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------
gyFileSystem::gyFileSystem()
{
  AddSearchPath(ROOT_FOLDER);
  writingPath.resolver = new FileResolverNative( ROOT_FOLDER );
}

gyFileSystem::~gyFileSystem()
{
  CloseAllFiles();
  ClearAllSearchPaths();
  SafeDestroyDelete(writingPath.resolver);
}

int gyFileSystem::Create(gyVarDict& params)
{
  if ( params.IsType("paths",gyVariant::T_VLIST) )
  {
    const gyVarList& vl = params["paths"].GetVL();
    for (int i = 0; i < vl.Size(); ++i )
      AddSearchPath(vl.Get(i).GetS());
  }

  if ( params.IsType("writingpath", gyVariant::T_STRING) )
  {
    SetWritingPath( params["writingpath"].GetS() );
  }
  return R_OK;
}

void gyFileSystem::CloseAllFiles()
{
  for ( size_t i = 0; i < filesArray.size(); ++i )
    SafeDestroyDelete( filesArray[i].file );
}
void gyFileSystem::ClearAllSearchPaths()
{
  for ( size_t i = 0; i < searchPaths.size(); ++i )
    SafeDestroyDelete( searchPaths[i].resolver );
}

void gyFileSystem::AddSearchPath( const char* spath, gyFileResolver* fileResolver/*=NULL*/ )
{
  if ( !spath || !*spath )
    return;

  // exists?
  for ( size_t i = 0; i < searchPaths.size(); ++i )
  {
    if ( searchPaths[i].resolver && gyStringUtil::StrCmp(spath, searchPaths[i].resolver->GetPath())==0 )
      return;
  }

  if ( !fileResolver )
    fileResolver = new FileResolverNative(spath);
  SearchPathEntry entry;
  entry.resolver = fileResolver;
  searchPaths.push_back(entry);
}

void gyFileSystem::SetWritingPath( const char* wpath )
{
  FAIL_IF( !writingPath.resolver, "Invalid writing path resolver" );
  if ( !wpath || !*wpath ) 
    wpath = ROOT_FOLDER;
  static_cast<FileResolverNative*>(writingPath.resolver)->SetNewPath(wpath);
}

gyIDFile gyFileSystem::FileOpen( const char* name, eCreateOptions op/* =OP_READ */ )
{
  // -- search for the file in the search paths (depends if reading or writing)
  FileEntry fentry;
  gyFileResolver* fresolver=NULL;
  if ( op == OP_READ )
  {
    // if reading, go through all search paths and get a proper resolver
    fentry.spIndex = FindFile(name);
    FAIL_IF( fentry.spIndex==-1, gyStrFormat256("Cannot find file '%s'",name) );    
    fresolver = searchPaths[fentry.spIndex].resolver;
  }
  else if ( op == OP_WRITE )
  {
    // if writing, get the only writing resolver
    fresolver = writingPath.resolver;
    fentry.spIndex = -1;
  }

  // finally, if resolver was found, then open the file
  FAIL_IF( !fresolver, "Cannot set a resolver for this file" );
  fentry.file = fresolver->FileOpen(name,op);
  FAIL_IF( !fentry.file, gyStrFormat256("Cannot open file '%s'", name ) );
  
  // -- we're here, so the file was correctly found and created
  gyIDFile fid;
  fid.Number((uint32_t)filesArray.size());
  filesArray.push_back(fentry);
  return fid;
}

int gyFileSystem::FindFile( const char* name )
{
  for ( size_t i = 0; i < searchPaths.size(); ++i )
  {
    SearchPathEntry& searchPath = searchPaths[i];
    if ( !searchPath.resolver ) 
      return -1;
    if ( searchPath.resolver->FileExists(name) )
      return (int)i;
  }
  return -1;
}

const char* gyFileSystem::GetSearchPath(int index)
{
  FAIL_IF( index < 0 || index >= (int)searchPaths.size(), "out-of-bounds");
  SearchPathEntry& sp = searchPaths[index];
  FAIL_IF( !sp.resolver, "Invalid resolver for this search path" );
  return sp.resolver->GetPath();
}
#define CHECK_AND_GET_FILEID(fid) if ( !fid.IsValid() ) return; \
                                  FileEntry& fentry = filesArray[fid.Number()];\
                                  FAIL_IF( !fentry.file, "Valid FileID but invalid file object" );
#define CHECK_AND_GET_FILEID_RET(fid,r) if ( !fid.IsValid() ) return r; \
                                      FileEntry& fentry = filesArray[fid.Number()];\
                                      FAIL_IF( !fentry.file, "Valid FileID but invalid file object" );
void gyFileSystem::FileClose ( gyIDFile& fid )
{
  CHECK_AND_GET_FILEID(fid);
  SafeDestroyDelete(fentry.file);
  
  // set this slot with last
  filesArray[fid.Number()] = filesArray.back();
  filesArray.pop_back();
  fid = gyIDFile::INVALID();
}

uint64_t gyFileSystem::FileLength( gyIDFile fid )
{
  CHECK_AND_GET_FILEID_RET(fid,0);
  uint64_t cur = FileTell( fid );
  FileSeek(fid,0,gyFileSystem::SK_END);
  uint64_t len = FileTell( fid );
  FileSeek(fid,cur,gyFileSystem::SK_SET);
  return len;
}

uint64_t gyFileSystem::FileRead  ( gyIDFile fid, void* dstBuffer, uint64_t bytesToRead )
{
  CHECK_AND_GET_FILEID_RET(fid,0);
  if ( !fentry.file ) return 0;
  if ( fentry.file->GetCreateOptions() != OP_READ )
    GYDEBUGRET( "File not created for reading", 0 );
  return fentry.file->Read( dstBuffer, bytesToRead );
}
uint64_t gyFileSystem::FileWrite ( gyIDFile fid, const void* srcBuffer, uint64_t bytesToWrite )
{
  CHECK_AND_GET_FILEID_RET(fid,0);
  if ( !fentry.file ) return 0;
  if ( fentry.file->GetCreateOptions() != OP_WRITE )
    GYDEBUGRET( "File not created for writing", 0 );
  return fentry.file->Write( srcBuffer, bytesToWrite );
}
void gyFileSystem::FileSeek  ( gyIDFile fid, uint64_t offset, eSeek seekOption/*=SK_SET*/ )
{
  CHECK_AND_GET_FILEID(fid);

  if ( fentry.file )
    fentry.file->Seek( offset, seekOption );
}
const char* gyFileSystem::FileName  ( gyIDFile fid )
{
  CHECK_AND_GET_FILEID_RET(fid,"");
  return (fentry.file) ? fentry.file->GetFileName() : "";
}
uint64_t gyFileSystem::FileTell  ( gyIDFile fid )
{
  CHECK_AND_GET_FILEID_RET(fid,0);
  uint64_t cur = 0;
  if ( fentry.file )
    cur = fentry.file->Tell();
  return cur;
}

// *************************************************************************************************
// *************************************************************************************************
void gyFileSystemHelper::NormalizePath( const char* path, char* dstPath, int maxSize, int pathLength )
{
  FAIL_IF(!path || !dstPath || maxSize<=0 || pathLength<=0, "Invalid input data" );  
  
  int count = pathLength < maxSize ? pathLength : maxSize;
  int i = 0;
  for ( ; i < count; ++i )
  {
    dstPath[i] = (char)tolower(path[i]);
    if ( path[i] == '\\' ) 
      dstPath[i] = '/';
  }
  if ( i > 0 && i < maxSize && dstPath[i-1] != '/' )
    dstPath[i++] = '/';
  if ( i < maxSize )
    dstPath[i] = 0;
}

const char* gyFileSystemHelper::NormalizePath( std::string& outPath )
{
  if ( outPath.empty()) 
    return NULL;
  char tmp[512];
  NormalizePath( outPath.c_str(), tmp, 512, (int)outPath.length() );
  outPath = tmp;
  return outPath.c_str();
}
const char* gyFileSystemHelper::ComposeSearchPathAndFileName( int searchPath, const char* filename )
{
  const char* path = gyGetFileSystem().GetSearchPath(searchPath);
  return gyStrFormat256( "%s%s", path, filename );
}
const char* gyFileSystemHelper::ComposeSearchPathAndFileName( const char* filename )
{
  int searchPathIndex = gyGetFileSystem().FindFile(filename);
  FAIL_IF( searchPathIndex==-1, "Couldn't find the file in file system" );  
  return ComposeSearchPathAndFileName(searchPathIndex, filename);
}
int64_t gyFileSystemHelper::FileReadToEnd( const char* filename, void* dstBuffer, uint64_t maxBytes )
{
  R_FAIL_IF( !filename || !dstBuffer || !maxBytes, "Invalid input data" );
  gyFileSystem& fs = gyGetFileSystem();
  gyIDFile fid = fs.FileOpen(filename);
  R_FAIL_IF( !fid.IsValid(), "Coulnd't open file" );
  const int64_t ret = FileReadToEnd( fid, dstBuffer, maxBytes );
  fs.FileClose(fid);
  return ret;
}

int64_t gyFileSystemHelper::FileReadToEnd( gyIDFile fid, void* dstBuffer, uint64_t maxBytes )
{
  R_FAIL_IF( !fid.IsValid() || !dstBuffer || !maxBytes, "Invalid input data" );
  gyFileSystem& fs = gyGetFileSystem();
  // how many bytes from here to the end of file?
  uint64_t cur = fs.FileTell(fid);
  fs.FileSeek(fid,0,gyFileSystem::SK_END);
  uint64_t len = fs.FileTell(fid) - cur;
  if ( len > maxBytes )
  {
    GYDEBUG( gyStrFormat256("Trying to read %lld but only %lld bytes available in buffer",len,maxBytes) );
    len = maxBytes;
  }
  // re-set where we were.
  fs.FileSeek(fid,cur,gyFileSystem::SK_SET);
  uint64_t bytesRead = fs.FileRead(fid,dstBuffer,len);
  return (int64_t)bytesRead;
}


void gyFileSystem::FillCreateTemplate(gyVarDict& outTemplate)
{
  GY_DEFAULT_PARAM(outTemplate, "paths"       , gyVarList());
  GY_DEFAULT_PARAM(outTemplate, "writingpath" , "");
}
