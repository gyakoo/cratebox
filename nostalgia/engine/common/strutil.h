#ifndef _GYSTRUTIL_H_
#define _GYSTRUTIL_H_

class gyStringUtil
{
public:
  // Returns: String(0), Char(1), Number(2), Float(3), Double(4)
  static int GuessType( const char* str, char split=',' );
  static const wchar_t* ToWString( const char* str, wchar_t* dst, int lenDst );
  static const char* ToAString ( const wchar_t* str, char* dst, int lenDst );
  static const wchar_t* ToWString( const char* str, std::wstring& dst );
  static const char* ToAString ( const wchar_t* str, std::string& dst );
  static int64_t ParseNumber( const char* str );
  static float ParseFloat( const char* str );
  static double ParseDouble( const char* str );
  static int Split(char* str, std::vector<const char*>& outParts, char split=',');
  static int StrLen(const char* str);
  static int StrLen(const wchar_t* str);
  static int StrCmp(const char* a, const char* b);
  static int StrCmp(const wchar_t* a, const wchar_t* b);
  static int StrCmp(const char* a, const wchar_t* b);
  static void StrCpy(char* dst, const char* b);
  static char* StrDup(const char* str);
  static void StrCat(char* dst, const char* b);
};


#endif