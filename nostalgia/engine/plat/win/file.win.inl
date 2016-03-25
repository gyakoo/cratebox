// ------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------
FileNative::FileNative(const char* filename, gyFileSystem::eCreateOptions op ):file(NULL), createOps(op)
{
  const char* opmode="rb";
  if ( op == gyFileSystem::OP_WRITE ) 
    opmode="wb";
  fopen_s( &file, filename, opmode );
#ifdef _DEBUG
  fileName = filename;
#endif
}
void FileNative::Destroy()
{
  if ( file )
    fclose(file);
  file=NULL;
}
uint64_t FileNative::Tell()
{
  FAIL_IF(!file,"Invalid file");
  return (uint64_t)ftell(file);
}
void FileNative::Seek( uint64_t offset, gyFileSystem::eSeek so )
{
  FAIL_IF(!file,"Invalid file");
  int options[]={ SEEK_SET, SEEK_CUR, SEEK_END };
  fseek( file, (long)offset, options[so] );
}
uint64_t FileNative::Read( void* buf, uint64_t bytes )
{
  FAIL_IF(!file,"Invalid file");
  if ( createOps != gyFileSystem::OP_READ ) return 0;
  uint64_t ret = (uint64_t)fread(buf,1,(size_t)bytes,file);
  //FAIL_IF( ret != bytes, StringFormatST("Read %lld bytes, expected %lld",ret,bytes) );
  return ret;
}
uint64_t FileNative::Write( const void* buf, uint64_t bytes )
{
  FAIL_IF(!file,"Invalid file");
  if ( createOps != gyFileSystem::OP_WRITE ) return 0;
  uint64_t ret = (uint64_t)fwrite(buf, 1, (size_t)bytes, file );
  FAIL_IF( ret != bytes, gyStrFormat256("Wrote %lld bytes, expected %lld",ret,bytes) );
  return ret;
}
// ------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------
FileResolverNative::FileResolverNative( const char* path ) : path(path)
{ 
  gyFileSystemHelper::NormalizePath(this->path);
}
void FileResolverNative::Destroy(){}
bool FileResolverNative::FileExists( const char* file )
{
  char tmp[512];
  sprintf_s( tmp, "%s%s", path.c_str(), file );
  FILE* f=NULL;
  fopen_s(&f,tmp,"rb");
  if ( !f ) return false;
  fclose(f);
  return true;
}
File* FileResolverNative::FileOpen( const char* file, gyFileSystem::eCreateOptions op )
{
  char tmp[512];
  sprintf_s( tmp, "%s%s", path.c_str(), file );
  FileNative* filenative = new FileNative( tmp, op );
  if ( !filenative->file )
    SafeDestroyDelete(filenative);
  return filenative;
}