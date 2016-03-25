#include <stdafx.h>
#include <engine/common/strutil.h>
#pragma warning(disable:4996)
#ifdef BX_PLATFORM_WINDOWS

const wchar_t* gyStringUtil::ToWString( const char* str, wchar_t* dst, int lenDst )
{
  MultiByteToWideChar(CP_ACP, 0, str, -1, dst, lenDst);
  return dst;
}
const char* gyStringUtil::ToAString ( const wchar_t* str, char* dst, int lenDst )
{
  WideCharToMultiByte(CP_ACP, 0, str, -1, dst, lenDst, NULL, NULL );
  return dst;
}
#endif


const wchar_t* gyStringUtil::ToWString( const char* str, std::wstring& dst )
{
  const int l = StrLen(str);
  if ( l > 0 )
  {
    dst.reserve(l+4);
    ToWString(str, const_cast<wchar_t*>(dst.c_str()), l+4);
  }
  return dst.c_str();
}

const char* gyStringUtil::ToAString ( const wchar_t* str, std::string& dst )
{
  const int l = StrLen(str);
  if ( l > 0 )
  {
    dst.reserve(l+4);
    ToAString(str,const_cast<char*>(dst.c_str()),l+4);
  }
  return dst.c_str();
}

// ---------------------------------------------------------------------------
// String(0), Char(1), Number(2), Float(3), Double(4)
int gyStringUtil::GuessType( const char* str, char split )
{
  enum { T_STR = 0, T_CHAR, T_NUMBER, T_FLOAT, T_DOUBLE, T_LIST };

  FAIL_IF( !str, "string is null" );
  if ( *str != 0 && *(str+1) == 0 && isalpha(*str) )
    return T_CHAR;

  const char* buff = str;
  // checking if list
  while ( *buff != 0 ) { if ( *buff == split ) return T_LIST; ++buff; }

  // guessing other types
  int n = 0;
  bool haspoint = false;
  bool onlydigits = true;
  while ( *str != 0 )
  {
    if ( !isdigit(*str) )
    {
      if ( *str == '.' )
      {
        if ( haspoint )
        {
          onlydigits = false;
          break;                    // second point is found (is a string)
        }else
          haspoint = true;

      }else if ( *str == '-' )
      {
        if ( n > 0 )
        {
          onlydigits = false;
          break;
        }
      }else if ( *str == 'f' && *(str+1) == 0 && haspoint )
      {
        return T_FLOAT;                // terminates with a 'f' and has a point
      }else
      {
        onlydigits = false;
        break;                        // a non-digit and non-point is found (is a string)
      }
    }
    ++str;
    ++n;
  }
  // if only digits, depending if has point, is double, otherwise is a number (int)
  if ( onlydigits ) return haspoint ? T_DOUBLE : T_NUMBER;

  // if we fall here, it's a string
  return T_STR;
}

int64_t gyStringUtil::ParseNumber( const char* str )
{
  FAIL_IF( !str, "invalid" );
  return _atoi64(str);
}

float gyStringUtil::ParseFloat( const char* str )
{
  FAIL_IF( !str, "invalid" );
  return (float)atof( str );
}

double gyStringUtil::ParseDouble( const char* str )
{
  FAIL_IF( !str, "invalid" );
  return atof( str );
}

int gyStringUtil::Split(char* str, std::vector<const char*>& outParts, char split)
{
  R_FAIL_IF( !str, "invalid" );
  if ( !*str )
    return 0;

  char* ptr = str;  
  outParts.push_back(ptr);
  const char* lastPtr = NULL;
  while ( *ptr )
  {
    if ( *ptr == split )
    {
      *ptr = '\0';
      if ( lastPtr )
        outParts.push_back(lastPtr);
      lastPtr=ptr+1;
    }
    ++ptr;
  }
  if ( lastPtr && *lastPtr )
    outParts.push_back(lastPtr);
  return (int)outParts.size();
}

int gyStringUtil::StrLen(const char* str)
{
  return (str && *str) ? (int)strlen(str) : 0;
}

int gyStringUtil::StrLen(const wchar_t* str)
{
  return (str && *str) ? (int)wcslen(str) : 0;
}


int gyStringUtil::StrCmp(const char* a, const char* b)
{
  return strcmp(a,b);
}
int gyStringUtil::StrCmp(const wchar_t* a, const wchar_t* b)
{
  return wcscmp(a,b);
}
int gyStringUtil::StrCmp(const char* a, const wchar_t* b)
{
  std::string astr;
  return strcmp( a, ToAString(b,astr) );
}

char* gyStringUtil::StrDup(const char* str)
{
  if ( !str ) return NULL;
  return _strdup(str);
}

void gyStringUtil::StrCpy(char* dst, const char* b)
{
  if ( !dst|| !b ) return;
  strcpy(dst,b);  
}

void gyStringUtil::StrCat(char* dst, const char* b)
{
  if ( !dst || !b ) return;
  strcat(dst,b);
}
#pragma warning(default:4996)
