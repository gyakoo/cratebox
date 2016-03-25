#include <thirdparty/fmod/inc/fmod.hpp>


/////////////////////////////////////////////////////////////////////////////////////////////
// In this class you'll put specific data related to a sound in FMOD
/////////////////////////////////////////////////////////////////////////////////////////////
struct gySoundFmod : public gySound
{
  gySoundFmod() : internalSound(NULL), channel(NULL)
  {
  }

  FMOD::Sound* internalSound;
  FMOD::Channel* channel;
};

/////////////////////////////////////////////////////////////////////////////////////////////
// This class in charge of general FMOD management
/////////////////////////////////////////////////////////////////////////////////////////////
class gySoundManager::Impl
{
public:
  Impl():system(NULL){ }
  ~Impl(){ Destroy(); }
  int Create(gyVarDict& createParams);
  void Destroy();
  void Update();
  void SetGlobalVolume(float vol);
  void SetPause(bool pause);
  bool IsPause();
  float GetGlobalVolume();
  gySound* SoundCreate(gySoundType soundType, const char* filename);
  void SoundPlay(gySound* sound);
  void SoundStop(gySound* sound);
  void SoundPause(gySound* sound, bool pause);
  float SoundGetInfo(gySound* sound, gySoundRunInfo* outInfo);
  float SoundGetVolume(gySound* sound);

private:
	FMOD::System	  *system;
	unsigned int	  version;
 
};

// Initialize FMOD system
int gySoundManager::Impl::Create(gyVarDict& createParams)
{
  // Call FMOD::System_Create, system->getVersion...	
  // See CHM documentation, getting started
  // You'll need to store the FMOD::System* system; as an attribute of the class gySoundManager::Impl
  // 
  // Return any error code != R_OK if fails
  FMOD_RESULT result;
	result = FMOD::System_Create(&system);
  if(result != FMOD_OK)
    return -1;

	result = system->getVersion(&version);
  if(result != FMOD_OK)
    return -1;

  void  *extradriverdata = NULL;

	result = system->init(32, FMOD_INIT_NORMAL, extradriverdata);
  if(result != FMOD_OK)
    return -1;

	return R_OK;
}

// Deinitialize FMOD system
void gySoundManager::Impl::Destroy()
{
  // call system->release()
  FMOD_RESULT result = system->release();
}

// FMOD has to be updated every frame, so call to system->update
void gySoundManager::Impl::Update()
{
  // call system->update()
	system->update();
}

// Set a global volume level
void gySoundManager::Impl::SetGlobalVolume(float vol)
{
	//channel->setVolume(vol);
  IMPLEMENT( "SoundMgr::SetGlobalVolume" );
  // See documentation+samples to check how to change the global volume
  // vol is normalized in 0..1. 
  // 0 is lowest volume level
  // 1 is maximum volume level
  // Maybe you have to convert from vol to scale from 0..1 to 0..255, don't know
}

// Global pause
void gySoundManager::Impl::SetPause(bool pause)
{
  IMPLEMENT( "SoundMgr::SetPause" );
  //channel->setPaused(pause);
  // if pause == true, pause the whole system
  // check documentation
}

// Returns if the system is in paused
bool gySoundManager::Impl::IsPause()
{
  IMPLEMENT( "SoundMgr::IsPause" );
  //return channel->getPaused();
  return false;
}

// Returns global volume
float gySoundManager::Impl::GetGlobalVolume()
{
  IMPLEMENT( "SoundMgr::GetGlobalVolume" );
  //return channel->getVolume();
  return 0;
}

// Creates an object of type gySoundFmod
// Uses soundType to select the corresponding flag in fmod when creating the sound
gySound* gySoundManager::Impl::SoundCreate(gySoundType soundType, const char* filename)
{
  gySoundFmod* sound = new gySoundFmod();
  sound->stringHash = gyComputeHash(filename, gyStringUtil::StrLen(filename));
  sound->soundType = soundType;
  
  const char* finalFilename = gyFileSystemHelper::ComposeSearchPathAndFileName(filename);

  FMOD_RESULT result;
  result = system->createSound(finalFilename, soundType, 0, &sound->internalSound);

  return sound;
}

// Plays the sound
void gySoundManager::Impl::SoundPlay(gySound* sound)
{
  gySoundFmod* fmodSound = (gySoundFmod*)sound;
  FMOD_RESULT result = system->playSound(fmodSound->internalSound, 0, false, &fmodSound->channel);  
}

// Stop the sound
void gySoundManager::Impl::SoundStop(gySound* sound)
{
  gySoundFmod* fmodSound = (gySoundFmod*)sound;
  if ( fmodSound->channel )
  {
    FMOD_RESULT result = fmodSound->channel->stop();
  }
}

void gySoundManager::Impl::SoundPause(gySound* sound, bool pause)
{
  gySoundFmod* fmodSound = (gySoundFmod*)sound;

  if ( fmodSound->channel )
  {
    FMOD_RESULT result = fmodSound->channel->setPaused(pause);
  }
}

float gySoundManager::Impl::SoundGetInfo(gySound* sound, gySoundRunInfo* outInfo)
{
  gySoundFmod* fmodSound = (gySoundFmod*)sound;
  IMPLEMENT("SoundGetInfo");
  return 0.0f;
}

float gySoundManager::Impl::SoundGetVolume(gySound* sound)
{
  gySoundFmod* fmodSound = (gySoundFmod*)sound;

  float volume=0.0f;
  if ( fmodSound->channel )
  {
    FMOD_RESULT result = fmodSound->channel->getVolume(&volume);
  }
  return volume;
}