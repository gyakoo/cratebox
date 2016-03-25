#include <gamePCH.h>
#include <engine/app/app.h>

class LevelEditorRenderer : public gyRenderContext
{
public:
  LevelEditorRenderer()
  {
    SetName( "LevelEditorRenderer" );    
  }

  gy_override void Destroy()
  {
  }

  gy_override void OnRender()
  {
    GY_PERFMARKER_SCOPE("LevelEditorRenderer");
    gyRenderer& renderer = gyGetRenderer();
    gyRenderCommandBuffer& pCtx = renderer.GetRenderConmmandBuffer(renderCommandBufferId);    
  }

  
protected:
};

// static void LoadEngineParams(const char* filename, gyVarDict& params)
// {
//   gyFileSystem& fs = gyGetFileSystem();
//   fs.AddSearchPath("../data/");
//   gyParser::JsonParse(filename,params);
// }

int RunLevelEditor(int64_t port)
{
  // Engine init
  gyVarDict params;
  //LoadEngineParams("levels/playlevel.json", params);
  if ( gyEngine::GetInstance().Create(params) != R_OK )
    return -1;

  // Renderer
  LevelEditorRenderer* renderer = new LevelEditorRenderer();
  gyGetRenderer().AddRenderContext( renderer );  

  // Main Loop
  while ( gyEngine::GetInstance().Run() == R_OK )
  {  
  }

  gyEngine::GetInstance().Release();
  return R_OK;
}

