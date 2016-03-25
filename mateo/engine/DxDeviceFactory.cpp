#include <Pch.h>
#include <engine/DxDeviceFactory.h>
#include <engine/DxDevice.h>
#include <engine/DxHelper.h>

using namespace Engine;
using namespace Concurrency;
using namespace DirectX;

#define ENGINE_D3DCOMPILER_DLL L"engine\\d3dcompiler_47.dll"

#define DXDEVFACTORY_EMIT_CREATESHADER(typetoken) \
{\
    ID3D11##typetoken* pSh = nullptr; \
    ThrowIfFailed(dxDev->Create##typetoken(byteCode.getPtr(), byteCode.getLength(), NULL, &pSh));\
    shader.shader = pSh;\
}

#define DXDEVFACTORY_EMIT_LOCKUNLOCK_IMPL(typetoken) \
Dx##typetoken& DxDeviceFactory::lock##typetoken(Id##typetoken id) \
{\
    return DxLocker<Dx##typetoken>(m_##typetoken##s).lock(id,m_##typetoken##Default);\
}\
void DxDeviceFactory::unlock##typetoken(Id##typetoken id)\
{\
	DxLocker<Dx##typetoken>(m_##typetoken##s).unlock(id);\
}

#define DXDEVFACTORY_EMIT_CREATECOMMON(typetoken, count, ...) \
{\
	typedef ID3D11##typetoken* (_cdecl CommonStates::*FactoryMethod)() const;\
	FactoryMethod facMethods[count] = {__VA_ARGS__};\
	Dx##typetoken state; \
	for (int i = 0; i < count; ++i)\
	{\
		state.state = DXSTATE_FIXED;\
		state.stateObj = ( m_commonStates.get()->*facMethods[i])();\
		m_##typetoken##s.push_back(state);\
		m_##typetoken##sCommon[i] = Id##typetoken(i);\
	}\
}

#define DXDEVFACTORY_EMIT_STATECREATEFINAL(typetoken, descptr) \
	Id##typetoken id((uint32_t)m_##typetoken##s.size());\
	Dx##typetoken state; \
	state.state = DXSTATE_LOADED; \
	ThrowIfFailed(DxDevice::GetInstance()->GetD3DDevice()->Create##typetoken(descptr, &state.stateObj)); \
	m_##typetoken##s.push_back(state);\
	return id;

#define DXDEVFACTORY_EMIT_FILLBUFFER(typetoken,_id) 	\
	auto& buffer = lock##typetoken##Buffer(_id);\
	D3D11_BOX updateBox = { 0, 0, 0, buffer.strideBytes * buffer.count, 1, 1 };\
	DxDevice::GetInstance()->GetD3DDeviceContext()->UpdateSubresource(buffer.buffer.Get(), 0, &updateBox, desc.data, 0, 0);\
	unlock##typetoken##Buffer(_id)

#define DXDEVFACTORY_EMIT_MAPBUFFER(typetoken,_id) \
	auto& buffer = lock##typetoken##Buffer(_id);\
	outMapping->count = buffer.count;\
	outMapping->strideBytes = buffer.strideBytes;\
	D3D11_MAPPED_SUBRESOURCE mapped;\
	ThrowIfFailed(DxDevice::GetInstance()->GetD3DDeviceContext()->Map(buffer.buffer.Get(), 0, mapType, 0, &mapped));\
	outMapping->data = mapped.pData;\
	unlock##typetoken##Buffer(_id)

template<typename CONT>
class DxLocker
{
public:
    DxLocker(concurrency::concurrent_vector<CONT>& container)
        : m_cont(container)
    {}

    template<typename IDTYPE>
    CONT& lock(IDTYPE id, CONT& def)
    {
        //ThrowIfAssert(id.IsValid(), L"id not valid");
        //ThrowIfAssert(id.Number() < m_cont.size(), L"id out of bounds");
        if (!id.IsValid() || id.Number()>=m_cont.size()) return def;
        
		CONT& retRes = m_cont[id.Number()];
		ThrowIfAssert(retRes.lockCount == 0, L"Locked already");
		++retRes.lockCount;
		return retRes;
    }

	template<typename IDTYPE>
	void unlock(IDTYPE id)
	{
		//ThrowIfAssert(id.IsValid(), L"id not valid");
		//ThrowIfAssert(id.Number() < m_cont.size(), L"id out of bounds");
        if (!id.IsValid() || id.Number() >= m_cont.size()) return;

		CONT& retRes = m_cont[id.Number()];
		--retRes.lockCount;
		ThrowIfAssert(retRes.lockCount >= 0, L"Too many unlocks");
	}

    concurrency::concurrent_vector<CONT>& m_cont;
};

DxDeviceFactory::DxDeviceFactory()
{
	createCommonStates();

    // compiler DLL to get the address of the reflector
    m_d3dDLLCompiler = LoadPackagedLibrary(ENGINE_D3DCOMPILER_DLL, 0);
    ThrowIfFailed(m_d3dDLLCompiler ? S_OK : E_FAIL, L"Cannot load " ENGINE_D3DCOMPILER_DLL);    
    m_d3dDLLReflect = (D3DReflectFunc)GetProcAddress(m_d3dDLLCompiler, "D3DReflect");
    ThrowIfFailed(m_d3dDLLReflect ? S_OK : E_FAIL, L"Cannot get proc address of D3dReflect");
}

DxDeviceFactory::~DxDeviceFactory()
{
    FreeLibrary(m_d3dDLLCompiler);
	releaseCommonStates();
    ThrowIfAssert(m_RenderTargets.empty());
    ThrowIfAssert(m_Textures.empty());
    ThrowIfAssert(m_VertexLayouts.empty());
    ThrowIfAssert(m_ByteCodes.empty());
    ThrowIfAssert(m_Shaders.empty());
    ThrowIfAssert(m_ConstantBuffers.empty());
    ThrowIfAssert(m_VertexBuffers.empty());
    ThrowIfAssert(m_IndexBuffers.empty());
    ThrowIfAssert(m_BlendStates.empty());
    ThrowIfAssert(m_DepthStencilStates.empty());
    ThrowIfAssert(m_RasterizerStates.empty());
    ThrowIfAssert(m_SamplerStates.empty());
}

void DxDeviceFactory::releaseResources()
{
}

IdRenderTarget  DxDeviceFactory::createRenderTarget(int32_t width, int32_t height, DXGI_FORMAT texf, bool asTexture, bool asDepth)
{
    ID3D11Device* pd3dDev = DxDevice::GetInstance()->GetD3DDevice();    
    DxRenderTarget renderTarget; ZeroMemory(&renderTarget, sizeof(DxRenderTarget));

    unsigned int bindFlags = D3D11_BIND_RENDER_TARGET;
    if (asTexture)
        bindFlags |= D3D11_BIND_SHADER_RESOURCE;

    // create the texture 2d for the target
    ThrowIfFailed(DxHelper::CreateEmptyTexture2D(width, height, texf, bindFlags, &renderTarget.texture));

    // Create the render target view (uses the texture as render target)
    ThrowIfFailed(pd3dDev->CreateRenderTargetView(renderTarget.texture.Get(), NULL, &renderTarget.renderTargetView));

    // Create the shader resource view if it will be used as texture in a shader
    if (asTexture)
        ThrowIfFailed(pd3dDev->CreateShaderResourceView(renderTarget.texture.Get(), NULL, &renderTarget.textureShaderResourceView));

    if (asDepth)
    {
        ThrowIfFailed(DxHelper::CreateEmptyTexture2D(width, height, texf, D3D11_BIND_DEPTH_STENCIL, &renderTarget.renderTargetDepthStencilTexture));
        ThrowIfFailed(pd3dDev->CreateDepthStencilView(renderTarget.renderTargetDepthStencilTexture.Get(), NULL, renderTarget.renderTargetDepthStencilView.GetAddressOf()));
    }

    IdRenderTarget retId( (uint32_t)m_RenderTargets.size());
	renderTarget.state = DXSTATE_LOADED;
	m_RenderTargets.push_back(renderTarget);
    return retId;
}

task<IdTexture> DxDeviceFactory::createTexture(const std::wstring& filename, uint32_t flags)
{
    return create_task(Engine::ReadDataAsync(filename))
        .then([this](const std::vector<byte>& fileData) -> IdTexture
    {
        IdTexture texId((uint32_t)m_Textures.size());
        DxTexture tex;
        tex.state = DXSTATE_LOADED;
        auto dxDev = DxDevice::GetInstance()->GetD3DDevice();
        ThrowIfFailed(CreateWICTextureFromMemory((ID3D11Device*)dxDev, (uint8_t*)&fileData[0], fileData.size(),
            (ID3D11Resource**)tex.texture.GetAddressOf(), tex.textureShaderResourceView.GetAddressOf()));
        m_Textures.push_back(tex);
        return texId;
    });	
}

IdTexture DxDeviceFactory::createTexture(IdRenderTarget rt)
{
    auto& rtTex = lockRenderTarget(rt);
    
	DxTexture texture;
	texture.state = DXSTATE_LOADED;
    texture.texture = rtTex.texture;
    texture.textureShaderResourceView = rtTex.textureShaderResourceView;

    IdTexture retId((uint32_t)m_Textures.size());
    m_Textures.push_back(texture);
    return retId;
}

IdTexture DxDeviceFactory::createTexture(DXGI_FORMAT texf, uint8_t* data, uint32_t width, uint32_t height, bool asDynamic)
{
    ThrowIfFailed(asDynamic ? E_FAIL : S_OK); // shout out if dynamic 

    // create texture with this description
    D3D11_TEXTURE2D_DESC desc = { 0 };
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.Format = texf;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    // initial data
    int32_t bpp = DxHelper::SizeOfFormatElement(texf);
    D3D11_SUBRESOURCE_DATA srdata;
    srdata.pSysMem = (void*)data;
    srdata.SysMemPitch = width * bpp;
    srdata.SysMemSlicePitch = width * height * bpp;

    // texture object with the pointers
	DxTexture texture;
	texture.state = DXSTATE_LOADED;

    // create resource and shader view
    auto dxDev = DxDevice::GetInstance()->GetD3DDevice();
    ThrowIfFailed(dxDev->CreateTexture2D(&desc, &srdata, &texture.texture));
    ThrowIfFailed( dxDev->CreateShaderResourceView(texture.texture.Get(), NULL, &texture.textureShaderResourceView) );

    // add to list
    IdTexture retId((uint32_t)m_Textures.size());
    m_Textures.push_back(texture);

    return retId;
}

IdVertexLayout  DxDeviceFactory::createVertexLayout(const std::vector<D3D11_INPUT_ELEMENT_DESC>& elements, IdByteCode bcId)
{
    ThrowIfFailed(elements.empty() ? E_FAIL : S_OK);
    ThrowIfFailed(!bcId.IsValid() ? E_FAIL : S_OK);

    auto dxDev = DxDevice::GetInstance()->GetD3DDevice();
    auto& byteCode = lockByteCode(bcId);
	DxVertexLayout vertexLayout;
	vertexLayout.state = DXSTATE_LOADED;
    ThrowIfFailed(dxDev->CreateInputLayout(&elements[0], (UINT)elements.size(), byteCode.getPtr(), (UINT)byteCode.getLength(), &vertexLayout.inputLayout) );
    unlockByteCode(bcId);
	
	// compute vertex stride in bytes
	for (auto elm : elements)
		vertexLayout.vertexStrideBytes += DxHelper::SizeOfFormatElement(elm.Format);

    IdVertexLayout retId((uint32_t)m_VertexLayouts.size());
    m_VertexLayouts.push_back(vertexLayout);
    return retId;
}

task<IdByteCode> DxDeviceFactory::createShaderByteCode(const std::wstring& filename)
{
    return create_task( Engine::ReadDataAsync(filename) )
        .then( [this](const std::vector<byte>& fileData) -> IdByteCode
    {
        IdByteCode bcId((uint32_t)m_ByteCodes.size());
        DxByteCode byteCode;
        byteCode.state = DXSTATE_LOADED;
        byteCode.data = std::make_shared<std::vector<byte>>(fileData);
        m_ByteCodes.push_back(byteCode);
        return bcId;
    });
}

IdByteCode DxDeviceFactory::createShaderByteCode(const std::vector<byte>& bytecode)
{
    IdByteCode bcId((uint32_t)m_ByteCodes.size());
    DxByteCode byteCode;
	byteCode.state = DXSTATE_LOADED;
    byteCode.data = std::make_shared<std::vector<byte>>(bytecode);
    m_ByteCodes.push_back(byteCode);
    return bcId;
}

IdShader DxDeviceFactory::createShader(IdByteCode byteCodeId, eDxShaderStage stage)
{
    auto dxDev = DxDevice::GetInstance()->GetD3DDevice();
    DxShader shader;

    auto& byteCode = lockByteCode(byteCodeId);
    switch (stage)
    {
    case SHADER_VERTEX  : DXDEVFACTORY_EMIT_CREATESHADER(VertexShader); break;
    case SHADER_HULL    : DXDEVFACTORY_EMIT_CREATESHADER(HullShader); break;
    case SHADER_DOMAIN  : DXDEVFACTORY_EMIT_CREATESHADER(DomainShader); break;
    case SHADER_GEOMETRY: DXDEVFACTORY_EMIT_CREATESHADER(GeometryShader); break;
    case SHADER_PIXEL   : DXDEVFACTORY_EMIT_CREATESHADER(PixelShader); break;
    case SHADER_COMPUTE : DXDEVFACTORY_EMIT_CREATESHADER(ComputeShader); break;
    default: ThrowIfFailed(E_FAIL); // unrecognized shader stage
    }
    unlockByteCode(byteCodeId);
	shader.state = DXSTATE_LOADED;
    IdShader shId((uint32_t)m_Shaders.size());
    m_Shaders.push_back(shader);
    return shId;
}

IdConstantBuffer DxDeviceFactory::createConstantBuffer(IdByteCode byteCode, eDxShaderStage stage)
{
    ThrowIfAssert(stage != SHADER_NONE);
    Microsoft::WRL::ComPtr<ID3D11ShaderReflection> reflector;
    
    // get the bytecode and reflect it
    auto& bcode = lockByteCode(byteCode);    
    ThrowIfFailed(m_d3dDLLReflect(bcode.getPtr(), bcode.getLength(), IID_ID3D11ShaderReflection, (void**)reflector.GetAddressOf()));
    unlockByteCode(byteCode);

    // creates a new constant buffer ...
    IdConstantBuffer cbId((uint32_t)m_ConstantBuffers.size());
    m_ConstantBuffers.push_back(DxConstantBuffer());
    DxConstantBuffer& cb = m_ConstantBuffers.back();
    cb.SetStage(stage);
    cb.CreateFromReflector(reflector.Get()); // ...which is initialized with the reflection.

    return cbId;
}

template<typename TID, typename TB>
TID createBufferInternal(concurrency::concurrent_vector<TB>& listBuffers, const DxMeshBufferElementDesc& desc, D3D11_BIND_FLAG bindFlag)
{
    TID id((uint32_t)listBuffers.size());
    TB buff;
    buff.state = DXSTATE_LOADED;
    listBuffers.push_back(buff);

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = desc.count * desc.strideBytes;
    bd.BindFlags = bindFlag;
    bd.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA* pInitData = NULL;
    D3D11_SUBRESOURCE_DATA idata;
    if (desc.data)
    {
        pInitData = &idata;
        pInitData->SysMemPitch = 0;
        pInitData->SysMemSlicePitch = 0;
        pInitData->pSysMem = desc.data;
    }

    ThrowIfFailed(DxDevice::GetInstance()->GetD3DDevice()->CreateBuffer(&bd, pInitData, buff.buffer.GetAddressOf()));

    buff.count = desc.count;
    buff.strideBytes = desc.strideBytes;
    return id;
}

IdVertexBuffer  DxDeviceFactory::createVertexBuffer(const DxMeshBufferElementDesc& vertexDesc)
{
    ThrowIfAssert(vertexDesc.IsValid());
    return createBufferInternal<IdVertexBuffer, DxVertexBuffer>(m_VertexBuffers, vertexDesc, D3D11_BIND_VERTEX_BUFFER);
}

IdIndexBuffer  DxDeviceFactory::createIndexBuffer(const DxMeshBufferElementDesc& indexDesc)
{
    ThrowIfAssert(indexDesc.IsValid());
    return createBufferInternal<IdIndexBuffer, DxIndexBuffer>(m_IndexBuffers, indexDesc, D3D11_BIND_INDEX_BUFFER);
}


void DxDeviceFactory::fillVertexBuffer(IdVertexBuffer vbId, const DxMeshBufferElementDesc& desc)
{
    DXDEVFACTORY_EMIT_FILLBUFFER(Vertex, vbId);
}

void DxDeviceFactory::fillIndexBuffer(IdIndexBuffer ibId, const DxMeshBufferElementDesc& desc)
{
    DXDEVFACTORY_EMIT_FILLBUFFER(Index, ibId);
}

void DxDeviceFactory::mapVertexBuffer(IdVertexBuffer vbId, DxMeshBufferElementDesc* outMapping, D3D11_MAP mapType)
{
    DXDEVFACTORY_EMIT_MAPBUFFER(Vertex, vbId);
}

void DxDeviceFactory::mapIndexBuffer(IdIndexBuffer ibId, DxMeshBufferElementDesc* outMapping, D3D11_MAP mapType)
{
    DXDEVFACTORY_EMIT_MAPBUFFER(Index, ibId);
}

void DxDeviceFactory::unmapVertexBuffer(IdVertexBuffer vbId)
{
    auto& buffer = lockVertexBuffer(vbId);
    DxDevice::GetInstance()->GetD3DDeviceContext()->Unmap(buffer.buffer.Get(), 0);
    unlockVertexBuffer(vbId);
}

void DxDeviceFactory::unmapIndexBuffer(IdIndexBuffer ibId)
{
    auto& buffer = lockIndexBuffer(ibId);
    DxDevice::GetInstance()->GetD3DDeviceContext()->Unmap(buffer.buffer.Get(), 0);
    unlockIndexBuffer(ibId);
}

IdBlendState DxDeviceFactory::createBlendState(D3D11_BLEND srcBlend, D3D11_BLEND destBlend)
{
	D3D11_BLEND_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	desc.RenderTarget[0].BlendEnable = (srcBlend != D3D11_BLEND_ONE) || (destBlend != D3D11_BLEND_ZERO);
	desc.RenderTarget[0].SrcBlend = desc.RenderTarget[0].SrcBlendAlpha = srcBlend;
	desc.RenderTarget[0].DestBlend = desc.RenderTarget[0].DestBlendAlpha = destBlend;
	desc.RenderTarget[0].BlendOp = desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	DXDEVFACTORY_EMIT_STATECREATEFINAL(BlendState, &desc);
}


IdDepthStencilState	DxDeviceFactory::createDepthStencilState(bool enable, bool writeEnable)
{
	D3D11_DEPTH_STENCIL_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	desc.DepthEnable = enable;
	desc.DepthWriteMask = writeEnable ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	desc.StencilEnable = false;
	desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

	desc.BackFace = desc.FrontFace;

	DXDEVFACTORY_EMIT_STATECREATEFINAL(DepthStencilState, &desc);
}

IdRasterizerState DxDeviceFactory::createRasterizerState(D3D11_CULL_MODE cullMode, D3D11_FILL_MODE fillMode)
{
	D3D11_RASTERIZER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	desc.CullMode = cullMode;
	desc.FillMode = fillMode;
	desc.DepthClipEnable = true;
	desc.MultisampleEnable = true;

	DXDEVFACTORY_EMIT_STATECREATEFINAL(RasterizerState, &desc);
}

IdSamplerState DxDeviceFactory::createSamplerState(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressMode)
{
	D3D11_SAMPLER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	desc.Filter = filter;

	desc.AddressU = addressMode;
	desc.AddressV = addressMode;
	desc.AddressW = addressMode;
	desc.MaxAnisotropy = (DxDevice::GetInstance()->GetD3DDevice()->GetFeatureLevel() > D3D_FEATURE_LEVEL_9_1) ? 16 : 2;

	desc.MaxLOD = FLT_MAX;
	desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

	DXDEVFACTORY_EMIT_STATECREATEFINAL(SamplerState, &desc);
}

IdRenderTarget DxDeviceFactory::getCommonRenderTarget()
{
	return m_commonRenderTarget;
}

template<typename CONT>
void releaseResourceInternal(CONT& container, uint32_t n, bool forced=false)
{
	CONT::value_type& resource = container[n];
    
    if ( forced )
        ThrowIfAssertAlways(resource.state == DXSTATE_LOADED, L"Only can release LOADED resources");
    
    --resource.refCount;
    if (resource.refCount <= 0)
    {
        resource.Release();
        resource.state = DXSTATE_RELEASED;
    }
}

void DxDeviceFactory::releaseResource(uint32_t ResourceId)
{
	ThrowIfAssert(IdGeneric::CreateFrom(ResourceId).IsValid()); // check if valid resource
	const uint32_t resType = IdGeneric::ExtractType(ResourceId);
	const uint32_t resNumber = IdGeneric::ExtractNumber(ResourceId);

	switch (resType)
	{
	case ID_RENDERTARGET: releaseResourceInternal(m_RenderTargets, resNumber); break;
	case ID_VERTEXLAYOUT: releaseResourceInternal(m_VertexLayouts, resNumber); break;
    case ID_VERTEXBUFFER: releaseResourceInternal(m_VertexBuffers, resNumber); break;
    case ID_INDEXBUFFER: releaseResourceInternal(m_IndexBuffers, resNumber); break;
	case ID_TEXTURE: releaseResourceInternal(m_Textures, resNumber); break;
	case ID_BYTECODE: releaseResourceInternal(m_ByteCodes, resNumber); break;
	case ID_SHADER: releaseResourceInternal(m_Shaders, resNumber); break;
    case ID_CONSTANTBUFFER: releaseResourceInternal(m_ConstantBuffers, resNumber); break;
	case ID_SAMPLERSTATE: releaseResourceInternal(m_SamplerStates, resNumber); break;
	case ID_DEPTHSTENCILSTATE: releaseResourceInternal(m_DepthStencilStates, resNumber); break;
	case ID_RASTERIZERSTATE: releaseResourceInternal(m_RasterizerStates, resNumber); break;
	case ID_BLENDSTATE: releaseResourceInternal(m_BlendStates, resNumber); break;
	default:
		ThrowIfAssert(false, L"Unknown resource to be released in DxDeviceFactory");
	}
}

IdBlendState DxDeviceFactory::getCommonBlendState(eDxCommonBlendStateType blendStateType) { return m_BlendStatesCommon[blendStateType]; }
IdDepthStencilState	DxDeviceFactory::getCommonDepthStencilState(eDxCommonDepthStencilType depthStencilType) { return m_DepthStencilStatesCommon[depthStencilType]; }
IdRasterizerState DxDeviceFactory::getCommonRasterizerState(eDxCommonRasterizerType rasterizerType) { return m_RasterizerStatesCommon[rasterizerType]; }
IdSamplerState DxDeviceFactory::getCommonSamplerState(eDxCommonSamplerType samplerType) { return m_SamplerStatesCommon[samplerType]; }

void DxDeviceFactory::createCommonStates()
{
	using namespace DirectX;
	auto dxDev = DxDevice::GetInstance();
    m_commonStates = std::make_unique<DirectX::CommonStates>(dxDev->GetD3DDevice());
	
	// blend states
	DXDEVFACTORY_EMIT_CREATECOMMON(BlendState, COMMONBLEND_MAX, &CommonStates::Opaque, &CommonStates::AlphaBlend, &CommonStates::Additive, &CommonStates::NonPremultiplied);
	DXDEVFACTORY_EMIT_CREATECOMMON(DepthStencilState, COMMONDEPTHSTENCIL_MAX, &CommonStates::DepthNone, &CommonStates::DepthDefault, &CommonStates::DepthRead);
	DXDEVFACTORY_EMIT_CREATECOMMON(RasterizerState, COMMONRASTERIZER_MAX, &CommonStates::CullNone, &CommonStates::CullClockwise, &CommonStates::CullCounterClockwise, &CommonStates::Wireframe);
	DXDEVFACTORY_EMIT_CREATECOMMON(SamplerState, COMMONSAMPLER_MAX, &CommonStates::PointWrap, &CommonStates::PointClamp, &CommonStates::LinearWrap, &CommonStates::LinearClamp, &CommonStates::AnisotropicWrap, &CommonStates::AnisotropicClamp );

	// gets a reference to the current render target and depth stencil views
	DxRenderTarget renderTarget;
	renderTarget.renderTargetView = dxDev->GetBackBufferRenderTargetView();
	renderTarget.renderTargetDepthStencilView = dxDev->GetDepthStencilView();

	m_commonRenderTarget.Number((uint32_t)m_RenderTargets.size());
	renderTarget.state = DXSTATE_FIXED;
	m_RenderTargets.push_back(renderTarget);
}

void DxDeviceFactory::releaseCommonStates()
{
    releaseResourceInternal(m_RenderTargets, IdGeneric::ExtractNumber(m_commonRenderTarget), true);
    for (auto rid : m_BlendStatesCommon)        releaseResourceInternal(m_BlendStates, IdGeneric::ExtractNumber(rid), true);
	for (auto rid : m_DepthStencilStatesCommon) releaseResourceInternal(m_DepthStencilStates,IdGeneric::ExtractNumber(rid),true);
	for (auto rid : m_RasterizerStatesCommon)   releaseResourceInternal(m_RasterizerStates, IdGeneric::ExtractNumber(rid), true);
	for (auto rid : m_SamplerStatesCommon)      releaseResourceInternal(m_SamplerStates, IdGeneric::ExtractNumber(rid), true);
    m_commonStates.reset();
}

DXDEVFACTORY_EMIT_LOCKUNLOCK_IMPL(RenderTarget);
DXDEVFACTORY_EMIT_LOCKUNLOCK_IMPL(Texture);
DXDEVFACTORY_EMIT_LOCKUNLOCK_IMPL(VertexLayout);
DXDEVFACTORY_EMIT_LOCKUNLOCK_IMPL(ByteCode);
DXDEVFACTORY_EMIT_LOCKUNLOCK_IMPL(Shader);
DXDEVFACTORY_EMIT_LOCKUNLOCK_IMPL(ConstantBuffer);
DXDEVFACTORY_EMIT_LOCKUNLOCK_IMPL(VertexBuffer);
DXDEVFACTORY_EMIT_LOCKUNLOCK_IMPL(IndexBuffer);
DXDEVFACTORY_EMIT_LOCKUNLOCK_IMPL(BlendState);
DXDEVFACTORY_EMIT_LOCKUNLOCK_IMPL(DepthStencilState);
DXDEVFACTORY_EMIT_LOCKUNLOCK_IMPL(RasterizerState);
DXDEVFACTORY_EMIT_LOCKUNLOCK_IMPL(SamplerState);

