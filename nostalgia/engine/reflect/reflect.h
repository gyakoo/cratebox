#ifndef _GY_REFLECT_H_
#define _GY_REFLECT_H_

class gyEngine;
class gyClass;
class gyReflectedObject;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#define GY_DEFAULT_PARAM(dict, param, defValue) { if ( !dict.Has(param) ) dict[param]=defValue; }
#define GY_DECLARE_REFLECT(classname) \
public: \
  static gyReflectedObject* CreateInstance(){ return new classname(); }\
  virtual const char* GetNameOfClass(){ return #classname; }

#define GY_REGISTER_REFLECT(classname) \
  namespace nsreflect##classname\
{\
struct classname##reg{ classname##reg(){ gyGetReflect().RegisterClass( #classname, classname::CreateInstance );} }reg;\
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class gyReflectedObject : public gyRefCounted
{
public:
  virtual const char* GetNameOfClass()=0;
  virtual gyClass* GetClass();
  virtual void FillCreateTemplate(gyVarDict& outTemplate){}
  virtual int Create(gyVarDict& createParams){return R_OK;}
};

typedef gyReflectedObject* (*GY_INSTANCE_CREATOR)();
class gyClass
{
public:
  std::string className;
  bool isAbstract;
  GY_INSTANCE_CREATOR create;

  gyReflectedObject* CreateInstance();
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class gyReflect : public gyReflectedObject
{
  GY_PIMPL_CLASS();
  GY_DECLARE_REFLECT(gyReflect);
public:
  gy_override int Create(gyVarDict& createParams);
  gy_override void FillCreateTemplate(gyVarDict& outTemplate);

  gyClass* RegisterClass(const char* classname, GY_INSTANCE_CREATOR create);
  gyReflectedObject* CreateInstance(const char* classname);
  gyClass* GetClassOfInstance(gyReflectedObject* instance);

private:
  friend class gyEngine;  
  friend class gyClass;
  gyReflect();
  ~gyReflect();
};

#endif