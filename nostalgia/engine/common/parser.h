#ifndef _GYPARSER_UTIL_H_
#define _GYPARSER_UTIL_H_

class gyParser
{
public:
  // high level json. dumps json into outDict. High memory footprint for large json!!
  static int JsonParse( char* jsonStr, gyVarDict& outDict);
  static int JsonParse( const char* filename, gyVarDict& outDict);

  // low level json. keeps internally the json 
  static gyIDJson JsonCreate(const char* jsonStr);
  static gyIDJson JsonCreateFromFile(const char* filename);
  static void JsonRelease(gyIDJson& idjson);
};


#endif