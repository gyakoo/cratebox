#include <gamePCH.h>
#include <engine/app/app.h>

struct gyShape2D
{
  void CreateRect( gyv2f& lefttop, gyv2f& size )
  {
    gyVertexElement elements[] = 
    {
      gyVertexElement( VES_POSITION,   0, FMT_R32G32B32A32_F, 0 ),
    };
    auto& rf = gyGetRendererFactory();
    vl = rf.CreateVertexLayout(elements,1);
    mb = rf.CreateMeshBuffer(vl, PT_TRILIST);

    gyMeshBuffer* pMb;
    if ( pMb = rf.LockMeshBuffer(mb) )
    {
      float values[6*4] = {0};
      gyv2 lt( -size.x*0.5f, size.y*0.5f );
      gyMath::TriangulateRect( lt, size, values, sizeof(float)*4 );
      pMb->CreateVertices(6, values);
      rf.UnlockMeshBuffer(mb);
    }
    lefttop.x += size.x*0.5f;
    lefttop.y -= size.y*0.5f;
    gyMath::MatrixTranslation(&transform, lefttop);
  }

  void Draw(gyRenderCommandBuffer& cmdBuf, uint32_t color)
  {
    // we asume vs/ps and cb are set outside
    cmdBuf.SetInputLayout(vl);
    cmdBuf.SetMeshBuffers(&mb,1);
    cmdBuf.Draw();
  }

  void Destroy()
  {
    gyGetRendererFactory().ReleaseResource(mb);
    gyGetRendererFactory().ReleaseResource(vl);
  }

  gymat44 transform;
  gyIDVertexLayout vl;
  gyIDMeshBuffer mb;
};

class LevelRenderer : public gyRenderContext
{
public:
  gyIDTexture stones;
  gyIDSamplerState sstate;
  gyIDBlendState bstate;
  gyShape2D shape;
  gyShaderTechnique shapeTech;
  gymat44 viewmat;
  gymat44 projmat;

  LevelRenderer()
  {
    SetName( "LevelRenderer" );    
    auto& rf = gyGetRendererFactory();
    stones = rf.CreateTexture(TT_TEXTURE2D,"stones.jpg");
    sstate = rf.CreateSamplerState(FT_ANISOTROPIC, AD_BORDER, AD_BORDER, AD_BORDER);
    bstate = rf.CreateBlendState(false, BLEND_SRC_ALPHA, BLEND_INV_SRC_ALPHA, BLENDOP_ADD);
    int64_t readBytes = gyFileSystemHelper::FileReadToEnd("base/Shape.fx",gyGetBigStackChunkST(), gyGetBigStackChunkSTSize());
    if ( readBytes > 0 )
    {
      gyShaderSourceDesc desc[]={ {SM_40, "VS", gyGetBigStackChunkST(), (uint32_t)readBytes}, {SM_40, "PS", gyGetBigStackChunkST(), (uint32_t)readBytes} };
      gyShaderType types[] = { ST_VERTEXSHADER, ST_PIXELSHADER };
      if ( rf.CreateShaderTechnique(&shapeTech, desc, types, 3) == R_OK )
        shape.CreateRect( gyv2f(-50.0f,10.0f), gyv2f(20.0f,20.0f) );
    }
    viewmat = gymat44::IDENTITY();
    gyMath::MatrixOrthographicProjection(&projmat, -50.0f, 50.0f, 50.0f, -50.0f, -1.0f, 1.0f);

    gyShaderConstantBuffer* pCB;
    pCB = rf.LockShader( shapeTech.GetStageShader(ST_VERTEXSHADER) )->GetConstantBuffer();
    pCB->SetConstantValue( pCB->FindConstantIndexByName("g_matP"), projmat.data, 16 );
    pCB->SetConstantValue( pCB->FindConstantIndexByName("g_matV"), viewmat.data, 16 );

    pCB = rf.LockShader( shapeTech.GetStageShader(ST_PIXELSHADER) )->GetConstantBuffer();
    float scolor[4]={1.0f,1,0,0.1f};
    pCB->SetConstantValue( pCB->FindConstantIndexByName("g_color"), scolor, 4 );
  }

  gy_override void Destroy()
  {
    auto& rf = gyGetRendererFactory();
    rf.ReleaseResource(stones);
    rf.ReleaseResource(sstate);
    rf.ReleaseResource(bstate);
    shapeTech.Destroy();
    shape.Destroy();
  }

  gy_override void OnRender()
  {
    GY_PERFMARKER_SCOPE("LevelRenderer");
    gyRenderer& renderer = gyGetRenderer();
    gyRenderCommandBuffer& cmdBuf = renderer.GetRenderConmmandBuffer(renderCommandBufferId);

    gyShaderConstantBuffer* pCB = gyGetRendererFactory().LockShader( shapeTech.GetStageShader(ST_VERTEXSHADER) )->GetConstantBuffer();
    pCB->SetConstantValue( pCB->FindConstantIndexByName("g_matM"), shape.transform.data, 16 );

    float ccolor[4];
    gyColor_PackedRGBAToFloatRGBA(0x779ecbff,ccolor);
    cmdBuf.ClearRenderTarget( NULL, 0, CS_CLEAR_ALL, ccolor);    
    cmdBuf.DrawQuadScreen(stones,sstate);

    cmdBuf.SetBlendState(bstate);
    cmdBuf.SetVSShader( shapeTech.GetStageShader(ST_VERTEXSHADER) );
    cmdBuf.SetVSShaderCB(shapeTech.GetStageShader(ST_VERTEXSHADER));

    cmdBuf.SetPSShader( shapeTech.GetStageShader(ST_PIXELSHADER) );
    cmdBuf.SetPSShaderCB( shapeTech.GetStageShader(ST_PIXELSHADER) );
    shape.Draw(cmdBuf, 0x779ecbff);
  }

  
protected:
};

static void LoadEngineParams(const char* filename, gyVarDict& params)
{
  gyGetFileSystem().AddSearchPath("../data/");
  gyParser::JsonParse(filename,params);
}


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

  // Input
  gyInput& input = gyGetInputSystem();
  gyIDInputTrigger ID_EXIT;
  ID_EXIT = input.TriggerCreate( GY_INPUTDEV_KEYBOARD, GYII_KB_ESC );
  input.TriggerCreate(ID_EXIT, GY_INPUTDEV_GAMEPAD_0, GYII_PAD_BACK_PAUSE);
    
  // Main Loop
  while ( gyEngine::GetInstance().Run() == R_OK )
  {
    if ( input.TriggerGet(ID_EXIT) != 0 )
      gyGetApplication().Terminate();    
  }

  gyEngine::GetInstance().Release();
  return R_OK;
}

