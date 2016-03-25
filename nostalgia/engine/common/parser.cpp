#include <stdafx.h>
#include <engine/common/parser.h>
#include <thirdparty/gason/gason.h>

// forward decls, mutual recursive
static void ParseJsonObject(JsonValue& value, gyVarDict& dict);
static void ParseJsonList(JsonValue& value, gyVarList& list);

static void ParseJsonList(JsonValue& value, gyVarList& list)
{
  if ( value.getTag() == JSON_NULL ) return;
  for ( auto i : value )
  {
    switch ( i->value.getTag() )
    {
    case JSON_NUMBER:
      list.Add(i->value.toNumber());
      break;
    case JSON_STRING:
      list.Add(i->value.toString());
      break;
    case JSON_TRUE:
    case JSON_FALSE:
      list.Add(i->value.getTag()==JSON_TRUE);
      break;
    case JSON_ARRAY:
      {
        gyVarList vl;
        ParseJsonList(i->value,vl);
        list.Add(vl);
      }break;
    case JSON_OBJECT:
      {
        gyVarDict vd;
        ParseJsonObject(i->value,vd);
        list.Add(vd);
      }break; 
    }
  }
}

static void ParseJsonObject(JsonValue& value, gyVarDict& dict)
{
  if ( value.getTag() == JSON_NULL ) return;

  for ( auto i : value )
  {
    switch ( i->value.getTag() )
    {
    case JSON_NUMBER:
      dict[i->key] = i->value.toNumber();
      break;
    case JSON_STRING:
      dict[i->key] = i->value.toString();
      break;
    case JSON_TRUE:
    case JSON_FALSE:
      dict[i->key] = i->value.getTag()==JSON_TRUE;
      break;
    case JSON_ARRAY:
      {
        gyVarList vl;
        ParseJsonList(i->value,vl);
        dict[i->key] = vl;
      }break;
    case JSON_OBJECT:
      {
        gyVarDict vd;
        ParseJsonObject(i->value,vd);
        dict[i->key] = vd;
      }break; 
    }
  }
}

struct gyJson : public gyResourceBase
{
  JsonAllocator allocator;
  JsonValue rootValue;
  char* str;
  uint32_t stringHash;
  char* endptr;
  ~gyJson()
  {
    SafeFree(str);
  }
  gy_override uint32_t ComputeHash(){ return stringHash; }
};

static gyResourceMgr<gyJson, ID_JSON> g_jsons;

int gyParser::JsonParse( char* jsonStr, gyVarDict& outDict)
{
  JsonAllocator allocator;
  JsonValue value;
  char* endptr;
  int status = jsonParse(jsonStr, &endptr, &value, allocator);
  if ( status != JSON_OK )
    return -1;

  // all json should start with an object (no key) "{...}"
  if ( value.getTag() == JSON_OBJECT )
    ParseJsonObject(value,outDict);

  return R_OK;
}


int gyParser::JsonParse( const char* filename, gyVarDict& outDict)
{
  uint32_t len = (uint32_t)gyFileSystemHelper::FileReadToEnd( filename, gyGetBigStackChunkST(), gyGetBigStackChunkSTSize() );
  if ( len == 0 ) 
    return -1;
  return JsonParse( (char*)gyGetBigStackChunkST(), outDict);
}

gyIDJson gyParser::JsonCreate(const char* jsonStr)
{
  if ( !jsonStr || *jsonStr==0 ) return gyIDJson::INVALID();

  gySharedPtr<gyJson> json = new gyJson;
  int len = gyStringUtil::StrLen(jsonStr);
  json->str = gyStringUtil::StrDup(jsonStr);
  json->stringHash = gyComputeHash(json->str,len);
  bool exists=false;
  gyIDJson idjson = g_jsons.AddUnique(json,&exists);
  if ( !exists )
  {
    int status = jsonParse(json->str,&json->endptr,&json->rootValue,json->allocator);
    if ( status != JSON_OK )
      g_jsons.Release(idjson);
  }
  return idjson;
}

gyIDJson gyParser::JsonCreateFromFile(const char* filename)
{
  uint32_t len = (uint32_t)gyFileSystemHelper::FileReadToEnd( filename, gyGetBigStackChunkST(), gyGetBigStackChunkSTSize() );
  if ( len == 0 ) 
    return gyIDJson::INVALID();
  return JsonCreate((char*)gyGetBigStackChunkST());
}

void gyParser::JsonRelease(gyIDJson& idjson)
{
  g_jsons.Release(idjson);
}


