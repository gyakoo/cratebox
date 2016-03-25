#pragma once
#include <engine/DxTypes.h>

namespace Engine
{
    // forwards
    class DxDeviceFactory;

    //
    // DxDeviceContextState
    //
    struct DxDeviceContextState
    {
        DxDeviceContextState();

        // RS
        D3D11_VIEWPORT m_RSViewport;
        IdRasterizerState m_RSRasterState;

        // IA
        D3D11_PRIMITIVE_TOPOLOGY m_IAPrimitiveTopology;
        IdVertexLayout m_IAVertexLayout;
        IdIndexBuffer m_IAIndexBuffer;
        std::array<IdVertexBuffer, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT> m_IAVertexBuffers;

        // PS
        std::array<IdSamplerState, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT> m_PSSamplerStates;
        std::array<IdTexture, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT> m_PSTextures;

        // OM states
        std::array<IdRenderTarget, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT> m_OMRenderTargets;
        IdRenderTarget m_OMDepthStencilTarget;
        IdBlendState m_OMBlendState;
        IdDepthStencilState m_OMDepthStencilState;

        // Shaders
        std::array<IdShader, SHADER_MAX> m_shaders;

        // ConsantBuffers
        std::array<IdConstantBuffer, SHADER_MAX> m_constantBuffers;

        bool operator ==(const DxDeviceContextState& rhs) const;
    };

    //
    // DxDeviceContext
    //
    class DxDeviceContext
    {
    public:
        DxDeviceContext(ID3D11DeviceContext* deviceContext);
        
		// RS
		void SetViewport(const D3D11_VIEWPORT& viewport);
		void SetRasterizerState(IdRasterizerState rasterizerState);

		// IA
		void SetInputLayout(IdVertexLayout vertexLayout);
		void SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology);
		void SetVertexBuffer(IdVertexBuffer vertexBuffer, uint32_t slot);
		void SetVertexBuffers(std::initializer_list<IdVertexBuffer> vertexBuffers, bool discardLeft = true);
        void SetIndexBuffer(IdIndexBuffer indexBuffer);
		
		// OM		
		void SetRenderTarget(std::initializer_list<IdRenderTarget> renderTargets, IdRenderTarget depthStencilTarget, bool discardLeft = true);
		void SetRenderTarget(IdRenderTarget renderTarget, uint32_t slot);
		void SetRenderTargetDefault();
		void SetBlendState(IdBlendState blendState);
		void SetDepthStencilState(IdDepthStencilState depthStencilState);

		// PS
		void SetSamplerStates(std::initializer_list<IdSamplerState> samplerStates, bool discardLeft=true);
		void SetSamplerState(IdSamplerState samplerState, uint32_t slot);
        void SetTextures(std::initializer_list<IdTexture> textures, bool discardLeft = true);
        void SetTexture(IdTexture texture, uint32_t slot);

        // Shader and Constant Buffers
		void SetShader(eDxShaderStage shaderType, IdShader shader);
        void SetConstantBuffer(IdConstantBuffer cbuffer, eDxShaderStage stage=SHADER_AUTO);
        
        // COMMANDS - Clear, Drawing
        void Apply();
        void CmdClearRenderTarget(std::initializer_list<IdRenderTarget> arrayRTs, const float* color, uint32_t depthStencilFlags, float depth = 1.0f, uint8_t stencil = 0);
        void CmdClearRenderTargetDefault(const float* color);
        void CmdDrawIndexed(uint32_t indexCount, uint32_t startIndexLocation=0, uint32_t baseVertexLocation=0);
        void CmdDraw(uint32_t vertexCount, uint32_t startVertexLocation = 0);
        void CmdDrawAuto();

        // Unbounding, resets
        void UnboundRenderTargets();

        // Debugging
        void MarkerBegin(const std::wstring& name);
        void MarkerEnd();

    private:
        void ApplyRS(ID3D11DeviceContext* context, DxDeviceFactory& factory);
        void ApplyIA(ID3D11DeviceContext* context, DxDeviceFactory& factory);
        void ApplyShaders(ID3D11DeviceContext* context, DxDeviceFactory& factory);
        void ApplyConstantBuffers(ID3D11DeviceContext* context, DxDeviceFactory& factory);
        void ApplyPS(ID3D11DeviceContext* context, DxDeviceFactory& factory);
        void ApplyOM(ID3D11DeviceContext* context, DxDeviceFactory& factory);

        Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_deviceContext;
        Microsoft::WRL::ComPtr<ID3DUserDefinedAnnotation> m_userAnnotation;

		DxDeviceContextState m_lastState;
		DxDeviceContextState m_currentState;
    };
};
