#include <stdafx.h>
#include <engine/reflect/reflect.h>
#include <new.h>
gyReflectedObject* gyClass::CreateInstance()
{
  if ( !create )
    return NULL;
  return create();
}

class gyReflect::Impl
{
public:
  ~Impl()
  {
    MapClasses::iterator it = classes.begin();
    for ( ; it!=classes.end(); ++it )
      SafeDelete(it->second);
    classes.clear();
  }

  gyReflectedObject* CreateInstance(const char* classname)
  {
    MapClasses::iterator it = classes.find(classname);
    if ( it == classes.end() || !it->second) 
      return NULL;
    return it->second->CreateInstance();
  }

  gyClass* RegisterClass(const char* classname, GY_INSTANCE_CREATOR create)
  {
    FAIL_IF( !create, "creator can't be null" );
    if ( !create ) return NULL;
    MapClasses::iterator it = classes.find(classname);
    gyClass* cls = NULL;
    if ( it != classes.end() )
    {
      cls = it->second;
    }
    else
    {
      cls = new gyClass;
      cls->className = classname;
      cls->isAbstract = false;
      cls->create = create;
      classes[classname] = cls;
    }
    return cls;
  }

  gyClass* GetClassOfInstance(gyReflectedObject* instance)
  {
    if ( !instance ) return NULL;
    MapClasses::iterator it = classes.find(instance->GetNameOfClass());
    return it != classes.end() ? it->second : NULL;
  }


  typedef std::map<std::string, gyClass*> MapClasses;
  MapClasses classes;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
gyClass* gyReflectedObject::GetClass()
{ 
  return gyGetReflect().GetClassOfInstance(this); 
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
int gyReflect::Create(gyVarDict& createParams)
{
  return R_OK;
}

void gyReflect::FillCreateTemplate(gyVarDict& outTemplate)
{
}

gyClass* gyReflect::RegisterClass(const char* classname, GY_INSTANCE_CREATOR create)
{
  return impl->RegisterClass(classname,create);
}

gyReflectedObject* gyReflect::CreateInstance(const char* classname)
{
  return impl->CreateInstance(classname);  
}

gyReflect::gyReflect()
{
  impl = new Impl();
}

gyReflect::~gyReflect()
{
  SafeDelete(impl);
}

gyClass* gyReflect::GetClassOfInstance(gyReflectedObject* instance)
{
  return impl->GetClassOfInstance(instance);
}



