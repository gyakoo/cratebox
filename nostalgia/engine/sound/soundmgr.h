#ifndef _GY_SOUNDMANAGER_H_
#define _GY_SOUNDMANAGER_H_

class gyEngine;

//////////////////////////////////////////////////////////////////////////
enum gySoundType
{
  GY_SOUND_UNKNOWN,
  GY_SOUND_2D,
  GY_SOUND_3D,
  GY_SOUND_STREAM
};

//////////////////////////////////////////////////////////////////////////
struct gySound : public gyResourceBase
{
  gySound():soundType(GY_SOUND_UNKNOWN), stringHash(0)
  {}

  gySoundType soundType;
  uint32_t stringHash;

  gy_override uint32_t ComputeHash();
};

struct gySoundRunInfo
{
  float volume;
};

//////////////////////////////////////////////////////////////////////////
class gySoundManager: public gyReflectedObject
{
  GY_PIMPL_CLASS();
  GY_DECLARE_REFLECT(gySoundManager);
public:
  gy_override int Create(gyVarDict& createParams);
  gy_override void FillCreateTemplate(gyVarDict& outTemplate);

  // -- Manager control
  void Destroy();
  void ReleaseAllSounds();
  void Update();
  void SetGlobalVolume(float vol);
  void SetPause(bool pause);
  bool IsPause();
  float GetGlobalVolume();
  
  // -- Sound API
  gyIDSound SoundCreate(gySoundType soundType, const char* filename);
  void SoundRelease(gyIDSound& idSound);
  void SoundPlay(gyIDSound idSound);
  void SoundStop(gyIDSound idSound);
  void SoundPause(gyIDSound idSound, bool pause);
  float SoundGetInfo(gyIDSound idSound, gySoundRunInfo* outInfo=NULL);
  float SoundGetVolume(gyIDSound idSound);

private:
  friend class gyEngine;
  gySoundManager();
  ~gySoundManager();

private:
  gyResourceMgr<gySound, ID_SOUND>  sounds;
};

#endif