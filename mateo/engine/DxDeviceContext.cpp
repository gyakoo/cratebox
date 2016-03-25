#include <Pch.h>
#include <engine/DxDeviceContext.h>
#include <engine/DxHelper.h>
#include <engine/DxDevice.h>

using namespace Engine;
using namespace Concurrency;
using namespace DirectX;

#define CONTEXT_STATE_DIFF(name) m_currentState.##name != m_lastState.##name
#define CONTEXT_EMIT_SET_SHADER(methodPrefix,stype,iface)  \
    if (CONTEXT_STATE_DIFF(m_shaders[stype]))\
    {\
        auto& sh = factory.lockShader(m_currentState.m_shaders[stype]);\
        context->##methodPrefix##SetShader((iface*)sh.shader.Get(), nullptr, 0);\
        factory.unlockShader(m_currentState.m_shaders[stype]);\
    }

template<typename SRCCONT, typename DSTCONT>
void SetArrayOfElements(SRCCONT& srcCont, DSTCONT& dstCont, bool discardLeft)
{
	size_t count = 0;
	size_t bound = std::min(srcCont.size(), dstCont.size());
	for (auto elm = srcCont.begin(); count < bound; ++elm, ++count)
	{
		dstCont[count] = *elm;
	}

	// discard the rest of elements ids?
	if (discardLeft)
	{
		for (auto i = bound; i < dstCont.size(); ++i)
		{
			dstCont[i].Invalidate();
		}
	}
}

DxDeviceContextState::DxDeviceContextState()
    : m_IAPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_UNDEFINED)
{
    ZeroMemory(&m_RSViewport, sizeof(D3D11_VIEWPORT));
}

bool DxDeviceContextState::operator ==(const DxDeviceContextState& rhs) const
{
    // try to order from more frequenly changed to less
    return m_constantBuffers == rhs.m_constantBuffers &&
        m_shaders == rhs.m_shaders &&
        m_IAVertexBuffers == rhs.m_IAVertexBuffers &&
        m_IAIndexBuffer == rhs.m_IAIndexBuffer &&
        m_PSTextures == rhs.m_PSTextures &&
        m_RSRasterState == rhs.m_RSRasterState &&
        m_IAPrimitiveTopology == rhs.m_IAPrimitiveTopology &&
        m_IAVertexLayout == rhs.m_IAVertexLayout &&
        m_PSSamplerStates == rhs.m_PSSamplerStates &&
        m_OMRenderTargets == rhs.m_OMRenderTargets &&
        m_OMDepthStencilTarget == rhs.m_OMDepthStencilTarget &&
        m_OMDepthStencilState == rhs.m_OMDepthStencilState &&
        m_OMBlendState == rhs.m_OMBlendState &&
        m_RSViewport == rhs.m_RSViewport;
}

DxDeviceContext::DxDeviceContext(ID3D11DeviceContext* deviceContext)
    : m_deviceContext(deviceContext)
{
#ifdef _DEBUG
    // for perf markers
    m_deviceContext.Get()->QueryInterface(IID_PPV_ARGS(m_userAnnotation.GetAddressOf()));
#endif
}

void DxDeviceContext::MarkerBegin(const std::wstring& name)
{
#ifdef _DEBUG
    m_userAnnotation->BeginEvent(name.c_str());
#endif
}

void DxDeviceContext::MarkerEnd()
{
#ifdef _DEBUG
    m_userAnnotation->EndEvent();
#endif
}

void DxDeviceContext::CmdClearRenderTarget(std::initializer_list<IdRenderTarget> arrayRTs, const float* color, uint32_t depthStencilFlags, float depth, uint8_t stencil)
{
	auto& factory = DxDevice::GetInstance()->GetFactory();

	for (auto rt = arrayRTs.begin(); rt != arrayRTs.end(); ++rt)
	{
		auto renderTarget = factory.lockRenderTarget(*rt);
		
		if (renderTarget.renderTargetView)
			m_deviceContext->ClearRenderTargetView(renderTarget.renderTargetView.Get(), color);

		if (depthStencilFlags != 0 && renderTarget.renderTargetDepthStencilView)
			m_deviceContext->ClearDepthStencilView(renderTarget.renderTargetDepthStencilView.Get(), depthStencilFlags, depth, stencil);
		
		factory.unlockRenderTarget(*rt);
	}
}

void DxDeviceContext::CmdClearRenderTargetDefault(const float* color)
{
	auto dxDev = DxDevice::GetInstance();
	
	auto backBufferRTV = dxDev->GetBackBufferRenderTargetView();
	m_deviceContext->ClearRenderTargetView(backBufferRTV, color);

	auto depthStencilV = dxDev->GetDepthStencilView();
	m_deviceContext->ClearDepthStencilView(depthStencilV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void DxDeviceContext::SetRenderTarget(std::initializer_list<IdRenderTarget> renderTargetIds, IdRenderTarget depthStencilTargetId, bool discardLeft)
{
	SetArrayOfElements(renderTargetIds, m_currentState.m_OMRenderTargets, discardLeft);
	m_currentState.m_OMDepthStencilTarget = depthStencilTargetId;
}

void DxDeviceContext::SetRenderTarget(IdRenderTarget renderTarget, uint32_t slot)
{
	m_currentState.m_OMRenderTargets[slot] = renderTarget;
}

void DxDeviceContext::SetRenderTargetDefault()
{
	auto commonRenderTarget = DxDevice::GetInstance()->GetFactory().getCommonRenderTarget();
	SetRenderTarget({ commonRenderTarget }, commonRenderTarget);
}

// RS
void DxDeviceContext::SetViewport(const D3D11_VIEWPORT& viewport)
{
	m_currentState.m_RSViewport = viewport;
}

// IA
void DxDeviceContext::SetInputLayout(IdVertexLayout vertexLayout)
{
	m_currentState.m_IAVertexLayout = vertexLayout;
}

void DxDeviceContext::SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology)
{
	m_currentState.m_IAPrimitiveTopology= topology;
}

void DxDeviceContext::SetVertexBuffer(IdVertexBuffer vertexBuffer, uint32_t slot)
{
    m_currentState.m_IAVertexBuffers[slot] = vertexBuffer;
}

void DxDeviceContext::SetVertexBuffers(std::initializer_list<IdVertexBuffer> vertexBuffers, bool discardLeft)
{
    SetArrayOfElements(vertexBuffers, m_currentState.m_IAVertexBuffers, discardLeft);
}

void DxDeviceContext::SetIndexBuffer(IdIndexBuffer indexBuffer)
{
    m_currentState.m_IAIndexBuffer = indexBuffer;
}

void DxDeviceContext::SetBlendState(IdBlendState blendState)
{
	m_currentState.m_OMBlendState = blendState;
}

void DxDeviceContext::SetDepthStencilState(IdDepthStencilState depthStencilState)
{
	m_currentState.m_OMDepthStencilState = depthStencilState;
}

void DxDeviceContext::SetRasterizerState(IdRasterizerState rasterizerState)
{
	m_currentState.m_RSRasterState = rasterizerState;
}

void DxDeviceContext::SetSamplerStates(std::initializer_list<IdSamplerState> samplerStates, bool discardLeft )
{
	SetArrayOfElements(samplerStates, m_currentState.m_PSSamplerStates, discardLeft);
}

void DxDeviceContext::SetSamplerState(IdSamplerState samplerState, uint32_t slot)
{
	m_currentState.m_PSSamplerStates[slot] = samplerState;
}

void DxDeviceContext::SetTextures(std::initializer_list<IdTexture> textures, bool discardLeft)
{
    SetArrayOfElements(textures, m_currentState.m_PSTextures, discardLeft);
}

void DxDeviceContext::SetTexture(IdTexture texture, uint32_t slot)
{
    m_currentState.m_PSTextures[slot] = texture;
}


void DxDeviceContext::SetShader(eDxShaderStage shaderType, IdShader shader)
{
	m_currentState.m_shaders[shaderType] = shader;
}

void DxDeviceContext::SetConstantBuffer(IdConstantBuffer cbuffer, eDxShaderStage stage)
{
    DxDeviceFactory& factory = DxDevice::GetInstance()->GetFactory();
    auto& cb = factory.lockConstantBuffer(cbuffer);
    if (stage == SHADER_AUTO)
    {
        stage = cb.GetStage();
    }
    else
    {
        ThrowIfAssert(stage != cb.GetStage(), L"Passed stage mismatch with ConstantBuffer's");
    }
    factory.unlockConstantBuffer(cbuffer);

    ThrowIfAssert(stage != SHADER_AUTO);
    m_currentState.m_constantBuffers[stage] = cbuffer;
}

void DxDeviceContext::ApplyRS(ID3D11DeviceContext* context, DxDeviceFactory& factory)
{
    // RS - VIEWPORT
    if (CONTEXT_STATE_DIFF(m_RSViewport))
    {
        context->RSSetViewports(1, &m_currentState.m_RSViewport);
    }

    // RS - RASTERIZER STATE
    if (CONTEXT_STATE_DIFF(m_RSRasterState))
    {
        auto& rs = factory.lockRasterizerState(m_currentState.m_RSRasterState);
        context->RSSetState(rs.stateObj.Get());
        factory.unlockRasterizerState(m_currentState.m_RSRasterState);
    }
}

void DxDeviceContext::ApplyIA(ID3D11DeviceContext* context, DxDeviceFactory& factory)
{
    // IA
    if (CONTEXT_STATE_DIFF(m_IAPrimitiveTopology))
        context->IASetPrimitiveTopology(m_currentState.m_IAPrimitiveTopology);

    if (CONTEXT_STATE_DIFF(m_IAVertexLayout))
    {
        auto& vl = factory.lockVertexLayout(m_currentState.m_IAVertexLayout);
        context->IASetInputLayout(vl.inputLayout.Get());
        factory.unlockVertexLayout(m_currentState.m_IAVertexLayout);
    }

    if (CONTEXT_STATE_DIFF(m_IAIndexBuffer))
    {
        auto& ib = factory.lockIndexBuffer(m_currentState.m_IAIndexBuffer);
        context->IASetIndexBuffer(ib.buffer.Get(), ib.strideBytes <= 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);
        factory.unlockIndexBuffer(m_currentState.m_IAIndexBuffer);
    }

    if (CONTEXT_STATE_DIFF(m_IAVertexBuffers))
    {
        std::array<ID3D11Buffer*, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT> d3dBuffers;
        std::array<UINT, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT> d3dBufferStrides;
        std::array<UINT, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT> d3dBufferOffsets = { 0 };
        auto& vbArray = m_currentState.m_IAVertexBuffers;
        IdVertexBuffer vbId;
        for (size_t i = 0; i < vbArray.size(); ++i)
        {
            vbId = vbArray[i];
            auto& vbRes = factory.lockVertexBuffer(vbId);
            d3dBuffers[i] = vbRes.buffer.Get();
            d3dBufferStrides[i] = vbRes.strideBytes;
            factory.unlockVertexBuffer(vbId);
        }
        context->IASetVertexBuffers(0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT, &d3dBuffers[0], &d3dBufferStrides[0], &d3dBufferOffsets[0]);
    }
}

void DxDeviceContext::ApplyShaders(ID3D11DeviceContext* context, DxDeviceFactory& factory)
{
    // SHADERS
    CONTEXT_EMIT_SET_SHADER(VS, SHADER_VERTEX, ID3D11VertexShader);
    CONTEXT_EMIT_SET_SHADER(HS, SHADER_HULL, ID3D11HullShader);
    CONTEXT_EMIT_SET_SHADER(DS, SHADER_DOMAIN, ID3D11DomainShader);
    CONTEXT_EMIT_SET_SHADER(GS, SHADER_GEOMETRY, ID3D11GeometryShader);
    CONTEXT_EMIT_SET_SHADER(PS, SHADER_PIXEL, ID3D11PixelShader);
}

void DxDeviceContext::ApplyConstantBuffers(ID3D11DeviceContext* context, DxDeviceFactory& factory)
{
    typedef void (STDMETHODCALLTYPE ID3D11DeviceContext::*d3dFunc)(UINT, UINT, ID3D11Buffer*const*);
    static std::array<d3dFunc, SHADER_MAX> d3dFunctions = 
    { &ID3D11DeviceContext::VSSetConstantBuffers, &ID3D11DeviceContext::HSSetConstantBuffers
    , &ID3D11DeviceContext::DSSetConstantBuffers, &ID3D11DeviceContext::GSSetConstantBuffers
    , &ID3D11DeviceContext::PSSetConstantBuffers };

    // for all shaders
    IdConstantBuffer cbId;
    for (uint32_t i = 0; i < SHADER_MAX; ++i )
    {
        // get general CB for shader (i)
        cbId = m_currentState.m_constantBuffers[i];
        DxConstantBuffer& cbGeneral = factory.lockConstantBuffer(cbId);
        if (cbGeneral.state != DXSTATE_INVALID)
        {
            // for all real CBs of this shader
            const uint32_t cbCount = cbGeneral.GetBuffersCount();
            for (uint32_t j = 0; j < cbCount; ++j)
            {
                ID3D11Buffer* pd3dBuffer = cbGeneral.GetD3DBuffer(j);
                // copy the data from cpu memory to ID3D11Buffer (if dirty)
                DxConstantBuffer::ConstantBuffer& cb = cbGeneral.GetBuffer(j);

                // if this CB was dirty, set to the device context
                if (cb.Flush(context, pd3dBuffer))
                    (context->*(d3dFunctions[i]))(cb.bindPoint, 1, &pd3dBuffer);
            }
        }
        factory.unlockConstantBuffer(cbId);
    }
}

void DxDeviceContext::ApplyPS(ID3D11DeviceContext* context, DxDeviceFactory& factory)
{
    // PS - SAMPLER STATES
    if (CONTEXT_STATE_DIFF(m_PSSamplerStates))
    {
        std::array<ID3D11SamplerState*, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT> d3dSamplers;
        auto& sampArray = m_currentState.m_PSSamplerStates;
        IdSamplerState ssId;
        for (size_t i = 0; i < sampArray.size(); ++i)
        {
            ssId = sampArray[i];
            auto& samplerState = factory.lockSamplerState(ssId);
            d3dSamplers[i] = samplerState.stateObj.Get();
            factory.lockSamplerState(ssId);
        }
        context->PSSetSamplers(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT, &d3dSamplers[0]);
    }

    // PS - TEXTURES (we go one-by-one for textures)
    for (uint32_t i = 0; i < D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT; ++i)
    {
        if (CONTEXT_STATE_DIFF(m_PSTextures[i]))
        {
            IdTexture tid = m_currentState.m_PSTextures[i];
            auto& tex = factory.lockTexture(tid);
            context->PSSetShaderResources(i, 1, tex.textureShaderResourceView.GetAddressOf());
            factory.unlockTexture(tid);
        }
    }
}

void DxDeviceContext::ApplyOM(ID3D11DeviceContext* context, DxDeviceFactory& factory)
{
    // OM - RENDERTARGET + Depth Stencil TARGETS
    if (CONTEXT_STATE_DIFF(m_OMRenderTargets) || CONTEXT_STATE_DIFF(m_OMDepthStencilTarget))
    {
        std::array<ID3D11RenderTargetView*, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT> rts;
        auto& rtArray = m_currentState.m_OMRenderTargets;
        IdRenderTarget rtId;
        for (size_t i = 0; i < rtArray.size(); ++i)
        {
            rtId = rtArray[i];
            auto& rt = factory.lockRenderTarget(rtId);
            rts[i] = rt.renderTargetView.Get();
            factory.unlockRenderTarget(rtArray[i]);
        }

        auto& rtds = factory.lockRenderTarget(m_currentState.m_OMDepthStencilTarget);
        ID3D11DepthStencilView* dsView = rtds.renderTargetDepthStencilView.Get();
        factory.unlockRenderTarget(m_currentState.m_OMDepthStencilTarget);
        context->OMSetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, &rts[0], dsView);
    }

    // OM - BLEND STATE
    if (CONTEXT_STATE_DIFF(m_OMBlendState))
    {
        auto& bs = factory.lockBlendState(m_currentState.m_OMBlendState);
        context->OMSetBlendState(bs.stateObj.Get(), NULL, 0xffffffff);
        factory.unlockBlendState(m_currentState.m_OMBlendState);
    }

    // OM - DEPTH STENCIL STATE
    if (CONTEXT_STATE_DIFF(m_OMDepthStencilState))
    {
        auto& ds = factory.lockDepthStencilState(m_currentState.m_OMDepthStencilState);
        context->OMSetDepthStencilState(ds.stateObj.Get(), 0xffffffff);
        factory.unlockDepthStencilState(m_currentState.m_OMDepthStencilState);
    }
}

// only sets the different states
void DxDeviceContext::Apply()
{
    // did anything change?
    if (m_currentState == m_lastState)
        return;

    // apply each pipeline stage states
    ID3D11DeviceContext* context = m_deviceContext.Get();
    DxDeviceFactory& factory = DxDevice::GetInstance()->GetFactory();

    ApplyRS(context, factory);
    ApplyShaders(context, factory);
    ApplyConstantBuffers(context, factory);
    ApplyIA(context, factory);
    ApplyPS(context, factory);
    ApplyOM(context, factory);
    
    // Saving state
    m_lastState = m_currentState;
}

void DxDeviceContext::UnboundRenderTargets()
{
    m_lastState.m_OMRenderTargets.fill(IdRenderTarget::INVALID());
    m_lastState.m_OMDepthStencilTarget = IdRenderTarget::INVALID();

    m_currentState.m_OMRenderTargets = m_lastState.m_OMRenderTargets;
    m_currentState.m_OMDepthStencilTarget = m_currentState.m_OMDepthStencilTarget;
}

void DxDeviceContext::CmdDrawIndexed(uint32_t indexCount, uint32_t startIndexLocation, uint32_t baseVertexLocation)
{
    ThrowIfAssert(m_currentState.m_IAIndexBuffer.IsValid(), L"No index buffer bound");
    
    Apply();
    m_deviceContext->DrawIndexed(indexCount, startIndexLocation, baseVertexLocation);
}

void DxDeviceContext::CmdDraw(uint32_t vertexCount, uint32_t startVertexLocation )
{
    Apply();
    m_deviceContext->Draw(vertexCount, startVertexLocation);
}

void DxDeviceContext::CmdDrawAuto()
{
    Apply();

    DxDeviceFactory& factory = DxDevice::GetInstance()->GetFactory();
    
    // if there's an IB, Draw Indexed
    if (m_currentState.m_IAIndexBuffer.IsValid())
    {
        // indexed draw
        auto& ib = factory.lockIndexBuffer(m_currentState.m_IAIndexBuffer);
        CmdDrawIndexed(ib.count, 0, 0);
        factory.unlockIndexBuffer(m_currentState.m_IAIndexBuffer);
    }
    else
    {
        // count minimum vertex count with all bound vertex buffers
        uint32_t minCount = 0xffffffff;
        for (auto& vbid : m_currentState.m_IAVertexBuffers)
        {
            auto& vb = factory.lockVertexBuffer(vbid);
            if (vb.count != 0 && vb.count < minCount)
                minCount = vb.count;
            factory.unlockVertexBuffer(vbid);
        }
        if (minCount != 0xffffffff)
            CmdDraw(minCount, 0);
    }
}