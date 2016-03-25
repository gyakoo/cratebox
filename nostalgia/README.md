# nostalgia
Nostalgia is a (work in progress) simple cross platform game engine specifically created for a 2'5D game.<br/>
It's based in handlers for all resources, rather than pointers.<br/>
A handler is a uint32_t with bits for resource type and number.<br/>
All modules are accesible through class <i>gyEngine</i><br/>.

## some abstract modules

```c++
class gyRenderer; // d3d11 driver implementation
class gyRendererFactory;
class gyFileSystem;
class gyJobManager;
class gyInput; // mouse, keyboard and xinput implementation
class gyApp;
class gyTimer;
class gyVarDict;
class gyReflect;
class gySoundManager; // fmod driver implementation
class gyDynLibManager;
class gyNetManager; // winsock implementation
class gyPhysicsManager2D; // box2d
class gyLogManager; // overlapped io implementation
```

## usage sample (play a single level)

This sample will load a single renderer (see game/playlevel.cpp) and create a sound. <br/>
It also creates two keyboard bindings to exit the application and one gamepad to play the sound.<br/>
Check also the gist: https://gist.github.com/gyakoo/aac4d3b91f3e35215f59#file-testing-20graphics-20driver-cpp<br/>

```c++
int PlaySingleLevel(const char* level)
{
  // Engine init
  gyVarDict params;
  LoadEngineParams("levels/engine.json", params);
  if ( gyEngine::GetInstance().Create(params) != R_OK )
    return -1;

  // Renderer
  LevelRenderer* renderer = new LevelRenderer();
  gyGetRenderer().AddRenderContext( renderer );  

  // Init sound
  gyIDSound SOUNDID_JAGUAR = gyGetSoundManager().SoundCreate(GY_SOUND_2D, "jaguar.wav");
  
  // Input
  gyInput& input = gyGetInputSystem();
  gyIDInputTrigger ID_EXIT, PLAYSOUND_ID;
  ID_EXIT = input.TriggerCreate( GY_INPUTDEV_KEYBOARD, GYII_KB_ESC );
  input.TriggerCreate(ID_EXIT, GY_INPUTDEV_GAMEPAD_0, GYII_PAD_BACK_PAUSE);
  PLAYSOUND_ID= input.TriggerCreate(GY_INPUTDEV_GAMEPAD_0, GYII_PAD_RPAD_RIGHT, gyInputTriggerOpts(true) );
    
  // Main Loop
  while ( gyEngine::GetInstance().Run() == R_OK )
  {
    if ( input.TriggerGet(ID_EXIT) != 0 )
      gyGetApplication().Terminate();    
      
    if ( input.TriggerGet(PLAYSOUND_ID) != 0 )
      gyGetSoundManager().SoundPlay(SOUNDID_JAGUAR);
  }

  gyEngine::GetInstance().Release();
  return R_OK;
}
```


## class gyEngine
This is the main class which gives access to all modules in the system.<br/>
```c++
class gyEngine
{
public:
  static gyEngine& GetInstance();
  gyEngine();
  ~gyEngine();

  int Create(const gyVarDict& params);
  void Release();
  int Run();

  gyRenderer& GetRenderer();
  gyFileSystem& GetFileSystem();
  gyJobManager& GetJobManager();
  gyInput& GetInputSystem();
  gyApp& GetApplication();
  gyTimer& GetTimer();
  gyReflect& GetReflection();
  gyVarDict& GetCommonDict();
  gySoundManager& GetSoundManager();
  gyDynLibManager& GetDynLibManager();
  gyNetManager& GetNetManager();
  gyPhysicsManager2D& GetPhysicsManager2D();
  gyLogManager& GetLogManager();

private:
  int PrepareCreateParams(gyVarDict& params);
  int CreateModules(gyVarDict& params);

  gyRenderer* renderer;
  gyFileSystem* fileSystem;
  gyJobManager* jobManager;
  gyInput* inputSystem;
  gyApp* application;
  gyTimer* timer;
  gyReflect* reflect;
  gyVarDict* commonDict;
  gySoundManager* soundManager;
  gyDynLibManager* dynLibManager;
  gyNetManager* netManager;
  gyPhysicsManager2D* physicsManager2D;
  gyLogManager* logManager;
};
```
