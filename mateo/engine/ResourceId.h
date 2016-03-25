#pragma once

namespace Engine
{
    template<int N>
    struct ResourceId
    {
        enum { TYPEMASK = 0xff000000, TYPESHIFT = 24, MAXNUMB = ~TYPEMASK }; // 8 bits type, 24 bits number
        ResourceId() { Invalidate(); }
        explicit ResourceId(uint32_t _number) { Invalidate(); Number(_number); }

        static inline ResourceId<N> CreateFrom(uint32_t n) { ResourceId<N> t; t.id = n; return t; }
        inline uint8_t   Type()const { return  (id&TYPEMASK) >> TYPESHIFT; }
        inline uint32_t  Number()const { return (id&(~TYPEMASK)); }
        inline void    Number(uint32_t n) { id = (id&TYPEMASK) | (n&(~TYPEMASK)); }
        inline bool    IsValid()const { return (id&(~TYPEMASK)) != (~TYPEMASK); }
        inline void    Invalidate() { id = (N << TYPESHIFT) | (~TYPEMASK); }
        inline bool    operator ==(const ResourceId<N>& o) const { return id == o.id; }
        inline operator uint32_t() { return id; }
        static ResourceId<N> INVALID() { return ResourceId<N>(); } // an invalid id has the 24 lsb to 1 
        static uint32_t ExtractType(uint32_t resId) { return (resId&TYPEMASK) >> TYPESHIFT; }
        static uint32_t ExtractNumber(uint32_t resId) { return (resId&(~TYPEMASK)); }
    private:
        uint32_t  id;
    };


    enum eResourceType
    {
        // BASE
        ID_GENERIC = 0,
        ID_FILE,
        ID_JSON,
        ID_LOGWRITER,

        // RENDERER
        ID_RENDERTARGET,
        ID_VERTEXLAYOUT,
        ID_VERTEXBUFFER,
        ID_INDEXBUFFER,
        ID_TEXTURE,
        ID_BYTECODE,
        ID_SHADER,
        ID_CONSTANTBUFFER,
        ID_SAMPLERSTATE,
        ID_DEPTHSTENCILSTATE,
        ID_RASTERIZERSTATE,
        ID_BLENDSTATE,

        // INPUT
        ID_INPUTTRIGGER,

        // JOB MANAGER
        ID_JOB,

        // SOUND MANAGER
        ID_SOUND,

        // DYNAMIC LIBRARIES
        ID_DYNLIB,

        // NETWORK
        ID_SOCKET,

        // PHYSICS

        ID_BASERESOURCEID_MAX
    };

    typedef ResourceId<ID_GENERIC>            IdGeneric;
    typedef ResourceId<ID_FILE>               IdFile;
    typedef ResourceId<ID_JSON>               IdJson;
    typedef ResourceId<ID_LOGWRITER>          IdLogWriter;

    typedef ResourceId<ID_RENDERTARGET>       IdRenderTarget;
    typedef ResourceId<ID_VERTEXLAYOUT>       IdVertexLayout;
    typedef ResourceId<ID_VERTEXBUFFER>       IdVertexBuffer;
    typedef ResourceId<ID_INDEXBUFFER>        IdIndexBuffer;
    typedef ResourceId<ID_TEXTURE>            IdTexture;
    typedef ResourceId<ID_BYTECODE>           IdByteCode;
    typedef ResourceId<ID_SHADER>             IdShader;
    typedef ResourceId<ID_CONSTANTBUFFER>     IdConstantBuffer;
    typedef ResourceId<ID_SAMPLERSTATE>       IdSamplerState;
    typedef ResourceId<ID_DEPTHSTENCILSTATE>  IdDepthStencilState;
    typedef ResourceId<ID_RASTERIZERSTATE>    IdRasterizerState;
    typedef ResourceId<ID_BLENDSTATE>         IdBlendState;

    typedef ResourceId<ID_INPUTTRIGGER>       IdInputTrigger;
    typedef ResourceId<ID_JOB>                IdJob;
    typedef ResourceId<ID_SOUND>              IdSound;
    typedef ResourceId<ID_DYNLIB>             IdDynLib;
    typedef ResourceId<ID_SOCKET>             IdSocket;


    template<typename T>
    std::size_t makeHash(const T& v)
    {
        return std::hash<T>()(v);
    }

    template<typename T, typename... Args>
    std::size_t makeHash(T first, Args ... args)
    {
        //auto n = sizeof...(Args);
        std::size_t h = std::hash<T>()(first) ^ (makeHash(args...) << 1);
        return h;
    }
};