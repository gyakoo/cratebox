#pragma once
#include <Engine/DxTypes.h>

#define DXDEVFACTORY_EMIT_LOCKUNLOCK_DECL(typetoken) \
        Dx##typetoken& lock##typetoken(Id##typetoken id);\
        void unlock##typetoken(Id##typetoken id);

#define DXDEVFACTORY_EMIT_STATE_ARRAYS(typetoken)\
        Dx##typetoken m_##typetoken##Default;\
		concurrency::concurrent_vector<Dx##typetoken> m_##typetoken##s;

#define DXDEVFACTORY_EMIT_STATE_ARRAYS_COMMON(typetoken, count)\
		DXDEVFACTORY_EMIT_STATE_ARRAYS(typetoken) \
		std::array<Id##typetoken, count> m_##typetoken##sCommon

typedef HRESULT(WINAPI *D3DReflectFunc)(LPCVOID pSrcData,
    SIZE_T SrcDataSize,
    REFIID pInterface,
    void** ppReflector);

namespace Engine
{
    	class DxDeviceFactory
	{
	public:
		DxDeviceFactory();
		~DxDeviceFactory();
		void  releaseResources();

		// Creation
		IdRenderTarget  createRenderTarget(int32_t width, int32_t height, DXGI_FORMAT texf, bool asTexture = true, bool asDepth = false);
		Concurrency::task<IdTexture> createTexture(const std::wstring& filename, uint32_t flags = 0);
		IdTexture       createTexture(IdRenderTarget rt);
		IdTexture       createTexture(DXGI_FORMAT texf, uint8_t* data, uint32_t width, uint32_t height, bool asDynamic = false);
		IdVertexLayout  createVertexLayout(const std::vector<D3D11_INPUT_ELEMENT_DESC>& elements, IdByteCode bytecode);
        Concurrency::task<IdByteCode> createShaderByteCode(const std::wstring& filename);
		IdByteCode      createShaderByteCode(const std::vector<byte>& bytecode);
		IdShader        createShader(IdByteCode byteCodeId, eDxShaderStage m_stage);
        IdConstantBuffer createConstantBuffer(IdByteCode byteCode, eDxShaderStage m_stage);
        IdVertexBuffer  createVertexBuffer(const DxMeshBufferElementDesc& vertexDesc);
        IdIndexBuffer   createIndexBuffer(const DxMeshBufferElementDesc& indexDesc);
		IdBlendState		createBlendState(D3D11_BLEND srcBlend, D3D11_BLEND destBlend);
		IdDepthStencilState	createDepthStencilState(bool enable, bool writeEnable);
		IdRasterizerState	createRasterizerState(D3D11_CULL_MODE cullMode, D3D11_FILL_MODE fillMode);
		IdSamplerState		createSamplerState(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressMode);

		void releaseResource(uint32_t ResourceId);

		IdBlendState		getCommonBlendState(eDxCommonBlendStateType blendStateType);
		IdDepthStencilState	getCommonDepthStencilState(eDxCommonDepthStencilType depthStencilType);
		IdRasterizerState	getCommonRasterizerState(eDxCommonRasterizerType rasterizerType);
		IdSamplerState		getCommonSamplerState(eDxCommonSamplerType samplerType);
		IdRenderTarget		getCommonRenderTarget();
        DirectX::CommonStates& getCommonStates() { return *m_commonStates; }

		void fillVertexBuffer(IdVertexBuffer vbId, const DxMeshBufferElementDesc& desc);
		void fillIndexBuffer(IdIndexBuffer ibId, const DxMeshBufferElementDesc& desc);
		void mapVertexBuffer(IdVertexBuffer vbId, DxMeshBufferElementDesc* outMapping, D3D11_MAP mapType=D3D11_MAP_WRITE);
		void mapIndexBuffer(IdIndexBuffer ibId, DxMeshBufferElementDesc* outMapping, D3D11_MAP mapType = D3D11_MAP_WRITE);
		void unmapVertexBuffer(IdVertexBuffer vbId);
		void unmapIndexBuffer(IdIndexBuffer ibId);

        // Accessing to internal resources 
		DXDEVFACTORY_EMIT_LOCKUNLOCK_DECL(RenderTarget);
		DXDEVFACTORY_EMIT_LOCKUNLOCK_DECL(Texture);
		DXDEVFACTORY_EMIT_LOCKUNLOCK_DECL(ByteCode);
		DXDEVFACTORY_EMIT_LOCKUNLOCK_DECL(VertexLayout);
		DXDEVFACTORY_EMIT_LOCKUNLOCK_DECL(Shader);
        DXDEVFACTORY_EMIT_LOCKUNLOCK_DECL(ConstantBuffer);
        DXDEVFACTORY_EMIT_LOCKUNLOCK_DECL(VertexBuffer);
        DXDEVFACTORY_EMIT_LOCKUNLOCK_DECL(IndexBuffer);
		DXDEVFACTORY_EMIT_LOCKUNLOCK_DECL(BlendState);
		DXDEVFACTORY_EMIT_LOCKUNLOCK_DECL(DepthStencilState);
		DXDEVFACTORY_EMIT_LOCKUNLOCK_DECL(RasterizerState);
        DXDEVFACTORY_EMIT_LOCKUNLOCK_DECL(SamplerState);

    protected:
		void createCommonStates();
		void releaseCommonStates();

	protected:
        DXDEVFACTORY_EMIT_STATE_ARRAYS(RenderTarget);
        DXDEVFACTORY_EMIT_STATE_ARRAYS(Texture);
        DXDEVFACTORY_EMIT_STATE_ARRAYS(VertexLayout);
        DXDEVFACTORY_EMIT_STATE_ARRAYS(ByteCode);
        DXDEVFACTORY_EMIT_STATE_ARRAYS(Shader);
        DXDEVFACTORY_EMIT_STATE_ARRAYS(ConstantBuffer);
        DXDEVFACTORY_EMIT_STATE_ARRAYS(VertexBuffer);
        DXDEVFACTORY_EMIT_STATE_ARRAYS(IndexBuffer);
		DXDEVFACTORY_EMIT_STATE_ARRAYS_COMMON(BlendState, COMMONBLEND_MAX);
		DXDEVFACTORY_EMIT_STATE_ARRAYS_COMMON(DepthStencilState, COMMONDEPTHSTENCIL_MAX);
		DXDEVFACTORY_EMIT_STATE_ARRAYS_COMMON(RasterizerState, COMMONRASTERIZER_MAX);
		DXDEVFACTORY_EMIT_STATE_ARRAYS_COMMON(SamplerState, COMMONSAMPLER_MAX);
		IdRenderTarget m_commonRenderTarget;
        HMODULE m_d3dDLLCompiler;
        D3DReflectFunc m_d3dDLLReflect;
        std::unique_ptr<DirectX::CommonStates> m_commonStates;
    };

};
