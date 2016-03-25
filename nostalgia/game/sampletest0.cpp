#include <gamePCH.h>
#include <engine/app/app.h>

class BasicRenderer : public gyRenderContext
{
public:
  enum { MAX_CHANNELS=4 };

public:
  BasicRenderer()
    : m_globalTime(0.0f), m_globalTimeReg(-1)
  {
    SetName( "BasicRenderer" );
    // Sampler to render quad on screen with PS result
    m_pointSamplerID = gyGetRenderer().GetFactory().CreateSamplerState(FT_MIN_MAG_MIP_POINT, AD_NONE, AD_NONE, AD_NONE );
  }

  gy_override void Destroy()
  {
    for ( int i = 0; i < MAX_CHANNELS; ++i )
    {
      gyReleaseResource(m_channelIDs[i]);
      gyReleaseResource(m_samplerIDs[i]);
    }
    gyReleaseResource(m_pointSamplerID);
    gyReleaseResource(m_pixelShaderID);
  }

  gy_override void OnRender()
  {
    GY_PERFMARKER_SCOPE("BasicRenderer");
    gyRenderer& renderer = gyGetRenderer();
    gyRenderCommandBuffer& pCtx = renderer.GetRenderConmmandBuffer(renderCommandBufferId);
    if ( m_pixelShaderID.IsValid() )
    {
      UpdateShaderPerFrameConstants();
      pCtx.DrawQuadScreen(m_pixelShaderID, m_pointSamplerID);
    }
  }

  void SetPixelShaderID( gyIDShader psID )
  {
    gyGetRenderer().GetFactory().ReleaseResource(m_pixelShaderID);
    m_pixelShaderID = psID;
    if ( m_pixelShaderID.IsValid() )
    {
      gyRendererFactory& factory= gyGetRenderer().GetFactory();
      gyShaderConstantBuffer* pPSCB = factory.LockShader(m_pixelShaderID)->GetConstantBuffer();
      m_globalTimeReg = pPSCB->FindConstantIndexByName("globalTime");
      UpdateShaderChannels();
    }
  }

  void SetChannelID( uint8_t chn, gyIDTexture cid=gyIDTexture::INVALID() )
  {
    if ( chn >= 4) return;
    gyReleaseResource(m_channelIDs[chn]);
    m_channelIDs[chn] = cid;
    UpdateShaderChannels();
  }

  void SetSamplerID( uint8_t chn, gyIDSamplerState sid=gyIDSamplerState::INVALID() )
  {
    if ( chn >= 4) return;
    gyReleaseResource(m_samplerIDs[chn]);
    m_samplerIDs[chn] = sid;
    UpdateShaderChannels();
  }

  void SetGlobalTime( float globalTime ){ m_globalTime = globalTime; }

  bool LoadPixelShader(const char* filename)
  {
    // check if file changed since last time
    static uint32_t lastHash = 0x0;
    uint32_t len = (uint32_t)gyFileSystemHelper::FileReadToEnd( filename, gyGetBigStackChunkST(), gyGetBigStackChunkSTSize() );
    if ( len == 0 )
      return false;
    uint32_t newhash = gyComputeHash(gyGetBigStackChunkST(), len);
    if ( lastHash == newhash )
      return false;
    lastHash = newhash;

    // changed, so compile it and create shader
    gyRendererFactory& factory = gyGetRenderer().GetFactory();
    gyShaderSourceDesc psSourceDesc = { SM_40, "PS", gyGetBigStackChunkST(), len };
    gySharedPtr<gyShaderByteCode> pbcode = factory.CompileShader( ST_PIXELSHADER, psSourceDesc );
    if ( !pbcode )
      return false;
    gyIDShader sid = factory.CreateShader( pbcode );
    if ( sid.IsValid() )
      SetPixelShaderID(sid);
    return sid.IsValid();
  }

protected:
  void UpdateShaderChannels()
  {
    if ( !m_pixelShaderID.IsValid() ) return;
    gyRendererFactory& factory= gyGetRenderer().GetFactory();
    gyShaderConstantBuffer* pPSCB = factory.LockShader(m_pixelShaderID)->GetConstantBuffer();
    int t=0,s=0;
    int count=pPSCB->GetConstantsCount();
    for ( int i=0; i<count && (t<4||s<4); ++i )
    {
      const gyShaderConstant& sc = pPSCB->GetConstant(i);
      if (sc.IsTexture())
      {
        pPSCB->SetConstantValue(i,m_channelIDs[t]);
        ++t;
      }
      else if (sc.IsSampler())
      {
        pPSCB->SetConstantValue(i,m_samplerIDs[s]);
        ++s;
      }
    }  
  }

  void UpdateShaderPerFrameConstants()
  {
    if ( m_globalTimeReg != -1 )
    {
      gyRendererFactory& factory= gyGetRenderer().GetFactory();
      gyShaderConstantBuffer* pPSCB = factory.LockShader(m_pixelShaderID)->GetConstantBuffer();
      pPSCB->SetConstantValue(m_globalTimeReg, &m_globalTime, 1);
    }
  }

protected:
  gyIDTexture m_channelIDs[MAX_CHANNELS];
  gyIDSamplerState m_samplerIDs[MAX_CHANNELS];
  gyIDSamplerState m_pointSamplerID;
  gyIDShader m_pixelShaderID;
  int32_t m_globalTimeReg;
  float m_globalTime;
};

void FillInitSettings(gyVarDict& params)
{
  // filesystem
  {
    gyVarDict fs;
    fs["paths"] = GYVARLIST("../data/", "../../data/");
    fs["writingpath"] = "../data/";
    params["gyFileSystem"] = fs;
  }

  // app
  {
    gyVarDict ap;
    gyVarDict window;
    window["title"] = GY_ENGINE_FULLNAME;
    window["width"] = 800;
    window["height"] = 600;
    window["windowed"] = true;
    ap["window"] = window;
    params["gyApp"] = ap;
  }

  params["appclass"] = "gyDefaultApp";
}


// TEST 0
int SampleTest0()
{
  gyVarDict params;
  FillInitSettings(params);
  
  if ( gyEngine::GetInstance().Create(params) != R_OK )
    return -1;

  // init specific renderer context
  BasicRenderer* renderer = new BasicRenderer();
  gyGetRenderer().AddRenderContext( renderer );
  gyRendererFactory& factory = gyGetRenderer().GetFactory();
  gyIDSamplerState linearSamplerID = factory.CreateSamplerState(FT_MIN_MAG_MIP_LINEAR, AD_MIRROR, AD_MIRROR, AD_NONE);
  gyIDTexture tID = factory.CreateTexture(TT_TEXTURE2D, "stones.jpg");
  renderer->SetChannelID(0, tID);
  renderer->SetSamplerID(0, linearSamplerID);
  renderer->LoadPixelShader("basic.fx");

  // init Input
  gyInput& input = gyGetInputSystem();
  gyIDInputTrigger ID_EXIT;
  ID_EXIT = input.TriggerCreate( GY_INPUTDEV_KEYBOARD, GYII_KB_ESC );
  input.TriggerCreate(ID_EXIT, GY_INPUTDEV_GAMEPAD_0, GYII_PAD_BACK_PAUSE);

  gyIDInputTrigger TOGGLEABSTIME_ID;
  TOGGLEABSTIME_ID = input.TriggerCreate( GY_INPUTDEV_KEYBOARD, GYII_KB_SPACE, gyInputTriggerOpts(true) );
  input.TriggerCreate( TOGGLEABSTIME_ID, GY_INPUTDEV_MOUSE, GYII_MO_WHEEL_DOWN );
  input.TriggerCreate( TOGGLEABSTIME_ID, GY_INPUTDEV_GAMEPAD_0, GYII_PAD_RPAD_DOWN, gyInputTriggerOpts(true) );

  gyIDInputTrigger DORUMBLE_ID = input.TriggerCreate(GY_INPUTDEV_GAMEPAD_0, GYII_PAD_RPAD_UP, gyInputTriggerOpts(true));
  
  // Init sound
  gyIDSound JAGUARSOUND_ID = gyGetSoundManager().SoundCreate(GY_SOUND_2D, "jaguar.wav");
  gyIDInputTrigger PLAYSOUND_ID= input.TriggerCreate(GY_INPUTDEV_KEYBOARD, GYII_KB_1, gyInputTriggerOpts(true) );
  input.TriggerCreate( PLAYSOUND_ID, GY_INPUTDEV_GAMEPAD_0, GYII_PAD_RPAD_RIGHT, gyInputTriggerOpts(true) );

  // Init sound2
  gyIDSound SWISHSOUND_ID = gyGetSoundManager().SoundCreate(GY_SOUND_2D, "swish.wav");
  gyIDInputTrigger PLAYSOUND2_ID= input.TriggerCreate(GY_INPUTDEV_KEYBOARD, GYII_KB_2, gyInputTriggerOpts(true) );
  input.TriggerCreate( PLAYSOUND2_ID, GY_INPUTDEV_GAMEPAD_0, GYII_PAD_RPAD_LEFT, gyInputTriggerOpts(true) );

  // Pause sound
  
  gyIDInputTrigger STOPSOUND_ID= input.TriggerCreate(GY_INPUTDEV_KEYBOARD, GYII_KB_3, gyInputTriggerOpts(true) );
  input.TriggerCreate( STOPSOUND_ID, GY_INPUTDEV_GAMEPAD_0, GYII_PAD_RPAD_RIGHT, gyInputTriggerOpts(true) );

  bool setGlobal=false;
  while ( gyEngine::GetInstance().Run() == R_OK )
  {
    if ( input.TriggerGet(ID_EXIT) != 0 )
      gyGetApplication().Terminate();

    if ( input.TriggerGet(TOGGLEABSTIME_ID) != 0 )
      setGlobal = !setGlobal;

    if ( setGlobal )
      renderer->SetGlobalTime( gyGetTimer().GetAbsoluteTime() );

    if ( input.TriggerGet(DORUMBLE_ID) != 0 )
    {
      gyInputPadVibrationFade fade(0.0f, 0.0f, 1.0f);
      input.SetPadVibration(GY_INPUTDEV_GAMEPAD_0, 1.0f, 1.0f, &fade);
    }

    if ( input.TriggerGet(PLAYSOUND_ID) != 0 )
      gyGetSoundManager().SoundPlay(JAGUARSOUND_ID);

    if ( input.TriggerGet(PLAYSOUND2_ID) != 0 )
      gyGetSoundManager().SoundPlay(SWISHSOUND_ID);

    if ( input.TriggerGet(STOPSOUND_ID) != 0 )
      gyGetSoundManager().SoundPause(JAGUARSOUND_ID, true);

  }

  gyEngine::GetInstance().Release();
  return R_OK;
}


// TEST 1
int SampleDLL()
{
  struct Functions
  {
    GY_DYNLIB_FUNCTION(int, filterNumber, (float));
    GY_DYNLIB_FUNCTION(int, fngentex, (void));
    GY_DYNLIB_FUNCTION(float, doSomething, (int, int, const char*));
  };

  /// -- test
  gyVarDict params;
  if ( gyEngine::GetInstance().Create(params) != R_OK ) return -1;

  // testing resource mgr
  gyIDDynLib gentexLib1 = gyGetDynLibManager().LibOpen("gentex.dll");
  if ( gentexLib1.IsValid() )
  {
    gySleep(100);

    Functions funcs;
    const char* funcSymbols[]={"filterNumber", "fngentex", "doSomething", NULL};
    if ( gyGetDynLibManager().LibGetFunctions( gentexLib1, (void*)&funcs, funcSymbols) )
    {
      int i = funcs.filterNumber(5.0f);
      int j = funcs.fngentex();
      float a = funcs.doSomething(i,j,"pepe");
      gySleep(100);
    }
    gyGetDynLibManager().LibClose(gentexLib1);
  }

  while ( gyEngine::GetInstance().Run() == R_OK ) ;
  gyEngine::GetInstance().Release();
  return R_OK;
}

// TEST 2
int SampleSocketsAndJSon()
{
  // init engine
  gyVarDict params;
  gyVarDict fs;
  fs["paths"] = GYVARLIST("../data/", "../../data/");
  fs["writingpath"] = "../data/";
  params["gyFileSystem"] = fs;
  if ( gyEngine::GetInstance().Create(params) != R_OK ) return -1;

  // socket
  auto& netMgr = gyGetNetManager();
  gyIDSocket sockFromTexTool = netMgr.SocketCreate(GY_SOCKET_TCP);
  if ( sockFromTexTool.IsValid() )
    netMgr.SocketListen(sockFromTexTool, 8888);

  // json
  {
    gyVarDict simCatalog;
    if ( gyParser::JsonParse("simple.json",simCatalog) == R_OK )
    {

    }
  }

  // more json
  {
    gyIDJson idjson = gyParser::JsonCreateFromFile("citm_catalog.json");
    gySleep(100);
    gyParser::JsonRelease(idjson);
  }

  while ( gyEngine::GetInstance().Run() == R_OK ) ;
  netMgr.SocketClose(sockFromTexTool);
  gyEngine::GetInstance().Release();
  return R_OK;
}

// TEST 3
int SamplePhysics2D()
{
  gyVarDict params;
  gyVarDict fs;
  fs["paths"] = GYVARLIST("../data/", "../../data/");
  fs["writingpath"] = "../data/";
  params["gyFileSystem"] = fs;

  gyVarDict p2d;
  p2d["gravX"] = 0.0f;
  p2d["gravY"] = -9.8f;
  params["gyPhysicsManager2D"] = p2d;
  if ( gyEngine::GetInstance().Create(params) != R_OK ) return -1;

  while ( gyEngine::GetInstance().Run() == R_OK ) ;
  gyEngine::GetInstance().Release();
  return R_OK;
}

// TEST 4
int SampleLogManager()
{
  // init engine
  gyVarDict params;
  gyVarDict fs;
  fs["paths"] = GYVARLIST("../data/", "../../data/");
  fs["writingpath"] = "../data/";
  params["gyFileSystem"] = fs;

  char logjson[]="{ \"loggers\": [ { \"type\": 0, \"format\":\"text\", \"filename\": \"../data/test.log\"} ] }";
  gyVarDict log;
  gyParser::JsonParse(logjson,log);
  params["gyLogManager"] = log;

  if ( gyEngine::GetInstance().Create(params) != R_OK ) return -1;
  
  GYINFO("Testing sample Log Manager...");
  GYWARNING( "Unfinished product." );
  GYDEBUG( "Stage %d", 5 );
  while ( gyEngine::GetInstance().Run() == R_OK ) ;
  gyEngine::GetInstance().Release();
  return R_OK;
}


int RunTest(int64_t test)
{
  typedef int (*testt)();
  testt tests[]={SampleTest0, SampleSocketsAndJSon, SamplePhysics2D, SampleDLL, SampleLogManager};
  if ( test >= 0 && test < sizeof(tests)/sizeof(testt) )
  {
    return tests[test]();
  }
  return -1;
}