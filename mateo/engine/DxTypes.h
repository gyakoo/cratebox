#pragma once

namespace Engine
{
#define DXDEVFACTORY_EMIT_STATE_DECL(typetoken) \
	struct Dx##typetoken: public DxResource \
	{ \
		Dx##typetoken() :stateObj(nullptr) {} \
		Microsoft::WRL::ComPtr<ID3D11##typetoken> stateObj; \
		void Release() { stateObj=nullptr; }\
	}

    enum eDxShaderStage
    {
        SHADER_NONE = -1,
        SHADER_AUTO = -1,
        SHADER_VERTEX = 0,
        SHADER_HULL,
        SHADER_DOMAIN,
        SHADER_GEOMETRY,
        SHADER_PIXEL,
        SHADER_COMPUTE,
        SHADER_MAX
    };

    enum eDxResourceState
    {
        DXSTATE_INVALID = 0,
        DXSTATE_LOADED,
        DXSTATE_LOADING,
        DXSTATE_RELEASED,
        DXSTATE_FIXED
    };

    enum eDxIndexFormat
    {
        INDEX_16 = 2,
        INDEX_32 = 4
    };

    enum eDxGeometryBufferType
    {
        GEOMTYPE_VERTEXBUFFER,
        GEOMTYPE_INDEXBUFFER
    };

    enum eDxShaderConstantType
    {
        // Do not modify number/orders!
        SCT_NONE = -1,
        SCT_STRING,                                                 // string
        SCT_FLOAT, SCT_FLOAT2, SCT_FLOAT3, SCT_FLOAT4,              // vector floats
        SCT_FLOAT33, SCT_FLOAT44, SCT_FLOAT43,                      // matrices
        SCT_TEXTURE, SCT_CUBEMAP, SCT_TEXTURE3D, SCT_TEXTURE2DARRAY,// textures
        SCT_BOOL, SCT_BOOL2, SCT_BOOL3, SCT_BOOL4,                  // vector bools
        SCT_INT, SCT_INT2, SCT_INT3, SCT_INT4, SCT_UINT,            // vector ints
        SCT_SAMPLER,                                                // sampler
    };

    enum eDxCommonBlendStateType 
    { 
        COMMONBLEND_OPAQUE = 0, 
        COMMONBLEND_ALPHABLEND, 
        COMMONBLEND_ADDITIVE, 
        COMMONBLEND_NONPREMULTIPLIED, 
        COMMONBLEND_MAX 
    };

    enum eDxCommonDepthStencilType 
    { 
        COMMONDEPTHSTENCIL_NONE = 0, 
        COMMONDEPTHSTENCIL_DEFAULT, 
        COMMONDEPTHSTENCIL_READ, 
        COMMONDEPTHSTENCIL_MAX 
    };

    enum eDxCommonRasterizerType 
    { 
        COMMONRASTERIZER_NONE = 0, 
        COMMONRASTERIZER_CULLCW, 
        COMMONRASTERIZER_CULLCCW, 
        COMMONRASTERIZER_WIREFRAME, 
        COMMONRASTERIZER_MAX 
    };

    enum eDxCommonSamplerType 
    { 
        COMMONSAMPLER_POINTWRAP = 0, 
        COMMONSAMPLER_POINTCLAMP, 
        COMMONSAMPLER_LINEARWRAP, 
        COMMONSAMPLER_LINEARCLAMP, 
        COMMONSAMPLER_ANISOWRAP, 
        COMMONSAMPLER_ANISOCLAMP, 
        COMMONSAMPLER_MAX 
    };

    interface DxLifetime
    {
    public:
        virtual void CreateResources() = 0;
        virtual void ReleaseResources() = 0;
        virtual void ReloadWindowSizeResources() = 0;
    };

    struct DxResource
    {
        DxResource() : state(DXSTATE_INVALID), hashValue(0), lockCount(0), refCount(1) {}

        eDxResourceState state;
        std::size_t hashValue;
        int32_t refCount;
        int32_t lockCount;
    };

    struct DxTexture : public DxResource
    {
        void Release()
        {
            texture = nullptr;
            textureShaderResourceView = nullptr;
        }

        Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureShaderResourceView;
    };

    struct DxRenderTarget : public DxTexture
    {
        void Release()
        {
            renderTargetView = nullptr;
            renderTargetDepthStencilTexture = nullptr;
            renderTargetDepthStencilView = nullptr;
        }

        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
        Microsoft::WRL::ComPtr<ID3D11Texture2D> renderTargetDepthStencilTexture;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilView> renderTargetDepthStencilView;
    };

    struct DxVertexLayout : public DxResource
    {
        DxVertexLayout() : vertexStrideBytes(0) {}
        void Release()
        {
            inputLayout = nullptr;
        }

        Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
        uint32_t vertexStrideBytes;
    };

    struct DxByteCode : public DxResource
    {
        std::shared_ptr< std::vector<byte> > data;
        void Release()
        {
            data = nullptr;
        }

        const void* getPtr() const { return &(*data.get())[0]; }
        uint32_t getLength() const { return (uint32_t)data->size(); }
    };

    struct DxShader : public DxResource
    {
        DxShader() : m_stage(SHADER_NONE) {}
        void Release()
        {
            shader = nullptr;
        }

        eDxShaderStage m_stage;
        Microsoft::WRL::ComPtr<ID3D11DeviceChild> shader;
    };

    //
    // DxGeometryBuffer
    //
    template<eDxGeometryBufferType GT>
    struct DxGeometryBuffer : public DxResource
    {
        enum { GEOM_TYPE = GT };
        DxGeometryBuffer() : count(0), strideBytes(0) { }
        void Release()
        {
            buffer = nullptr;
        }

        Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
        uint32_t count;
        uint32_t strideBytes;
    };

    typedef DxGeometryBuffer<GEOMTYPE_VERTEXBUFFER> DxVertexBuffer;

    typedef DxGeometryBuffer<GEOMTYPE_INDEXBUFFER> DxIndexBuffer;

    //
    // DxConstantBuffer
    //
    class DxConstantBuffer : public DxResource
    {
    public:
        struct ConstantBuffer
        {
            ConstantBuffer() : bindPoint(-1), sizeInBytes(0), cpuMemBuffer(nullptr), dirty(false) {}
            void Release();
            size_t GetSizeInBytes() const { return sizeInBytes; }
            bool Flush(ID3D11DeviceContext* context, ID3D11Buffer* d3dBuffer);

            float* cpuMemBuffer;
            int32_t bindPoint;
            uint32_t sizeInBytes;
            bool dirty;
        };

        struct ShaderConstant
        {
            ShaderConstant() : cbIndexAndOffset(~(-1)), nameHash(0), resNumber(0), type(0), sizeInBytes(0){}

            bool IsValid()const { return cbIndexAndOffset != -1 && type != SCT_NONE && sizeInBytes != 0 && nameHash != 0; }
            bool IsTexture()const { return type >= SCT_TEXTURE && type <= SCT_TEXTURE2DARRAY; }
            bool IsSampler()const { return type >= SCT_SAMPLER; }
            bool IsFloatArray()const { return type >= SCT_FLOAT && type <= SCT_FLOAT43; }

#ifdef _DEBUG
            std::string name;
#endif
            std::size_t nameHash;
            uint32_t cbIndexAndOffset;
            uint32_t resNumber;   // only used when IsTexture() or IsSampler(). it is the resource number
            uint16_t type;        // eDxShaderConstantType
            uint16_t sizeInBytes;
        };
    public:
        DxConstantBuffer() : m_stage(SHADER_NONE) {}
        void Release();
        void CreateFromReflector(ID3D11ShaderReflection* reflector);

        int32_t FindConstantIndexByName(const std::string& constantName) const;
        void SetConstantValue(int32_t constantNdx, const float* values, uint32_t count);
        void SetConstantValue(int32_t constantNdx, const bool* values, uint32_t count);
        void SetConstantValue(int32_t constantNdx, const int* values, uint32_t count);
        void SetConstantValue(int32_t constantNdx, IdTexture texture);
        void SetConstantValue(int32_t constantNdx, IdSamplerState samplerState);

        uint32_t GetBuffersCount() const { return (uint32_t)m_buffers.size(); }
        ConstantBuffer& GetBuffer(uint32_t ndx) { return m_buffers[ndx]; }
        uint32_t GetConstantsCount() const { return (uint32_t)m_constants.size(); }
        const ShaderConstant& GetConstant(int32_t cbIndexAndOffset) const { return m_constants[cbIndexAndOffset]; }

        ID3D11Buffer* GetD3DBuffer(uint32_t ndx) const { return m_d3dBuffers[ndx]; }

        void SetStage(eDxShaderStage stage) { m_stage = stage; }
        eDxShaderStage GetStage() { return m_stage; }
    protected:
        ConstantBuffer& GetCBIndexAndOffset(uint32_t constantIndex, uint32_t& outOffset);

        std::vector<ID3D11Buffer*> m_d3dBuffers;
        std::vector<ConstantBuffer> m_buffers;
        std::vector<ShaderConstant> m_constants; // textures at the beginning of this array
        eDxShaderStage m_stage;
    };

    struct DxMeshBufferElementDesc
    {
        void* data;
        uint32_t count;
        uint32_t strideBytes;

        bool IsValid()const { return data != nullptr && count > 0 && strideBytes > 0; }
    };

    DXDEVFACTORY_EMIT_STATE_DECL(BlendState);
    DXDEVFACTORY_EMIT_STATE_DECL(DepthStencilState);
    DXDEVFACTORY_EMIT_STATE_DECL(RasterizerState);
    DXDEVFACTORY_EMIT_STATE_DECL(SamplerState);
}