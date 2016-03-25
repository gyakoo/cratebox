#include <stdafx.h>
#include <engine/sound/soundmgr.h>

#include GY_PLATINC_SOUNDMGR

uint32_t gySound::ComputeHash()
{
  uint32_t data[]={stringHash, soundType};
  return gyComputeHash(data,sizeof(data));
}

int gySoundManager::Create(gyVarDict& createParams)
{
  return impl->Create(createParams);
}

void gySoundManager::FillCreateTemplate(gyVarDict& outTemplate)
{
  
}

gySoundManager::gySoundManager()
{
  impl = new Impl();
}

gySoundManager::~gySoundManager()
{
  Destroy();
  SafeDelete(impl);
}

void gySoundManager::Destroy()
{
  sounds.ReleaseAll();
  impl->Destroy();
}

void gySoundManager::Update()
{
  impl->Update();
}

void gySoundManager::SetGlobalVolume(float vol)
{
  impl->SetGlobalVolume(vol);
}

void gySoundManager::SetPause(bool pause)
{
  impl->SetPause(pause);
}

bool gySoundManager::IsPause()
{
  return impl->IsPause();
}

float gySoundManager::GetGlobalVolume()
{
  return impl->GetGlobalVolume();
}

void gySoundManager::ReleaseAllSounds()
{
  sounds.ReleaseAll();
}

gyIDSound gySoundManager::SoundCreate(gySoundType soundType, const char* filename)
{
  if ( !filename || soundType == GY_SOUND_UNKNOWN )
    return gyIDSound::INVALID();

  gySharedPtr<gySound> sound = impl->SoundCreate(soundType, filename);
  if ( !sound )
    return gyIDSound::INVALID();
  return sounds.AddUnique(sound); 
}

void gySoundManager::SoundRelease(gyIDSound& idSound)
{
  if ( !idSound.IsValid() )
    return;
  sounds.Release(idSound);
}

void gySoundManager::SoundPlay(gyIDSound idSound)
{
  gySound* sound = sounds.Get(idSound);
  if ( sound )
    impl->SoundPlay(sound);
}

void gySoundManager::SoundStop(gyIDSound idSound)
{
  gySound* sound = sounds.Get(idSound);
  if ( sound )
    impl->SoundStop(sound);
}

void gySoundManager::SoundPause(gyIDSound idSound, bool pause)
{
  gySound* sound = sounds.Get(idSound);
  if ( sound )
    impl->SoundPause(sound,pause);
}

float gySoundManager::SoundGetInfo(gyIDSound idSound, gySoundRunInfo* outInfo )
{
  gySound* sound = sounds.Get(idSound);
  if ( !sound )
    return 0.0f;

  return outInfo ? impl->SoundGetInfo(sound,outInfo) : impl->SoundGetVolume(sound);
}