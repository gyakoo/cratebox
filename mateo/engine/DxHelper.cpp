#include <pch.h>
#include <engine/DxHelper.h>
#include <engine/DxDevice.h>
#include <engine/DxDeviceContext.h>

using namespace Engine;
using namespace Engine::DxHelper;

int32_t DxHelper::CreateEmptyTexture2D(int32_t width, int32_t height, DXGI_FORMAT texf, uint32_t bindflags, ID3D11Texture2D** pOutTex)
{
    D3D11_TEXTURE2D_DESC descDepth;
    ZeroMemory(&descDepth, sizeof(descDepth));
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = texf;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = bindflags;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    return DxDevice::GetInstance()->GetD3DDevice()->CreateTexture2D(&descDepth, NULL, pOutTex);
}

int32_t DxHelper::SizeOfFormatElement(DXGI_FORMAT format)
{

    switch (format)
    {
    case DXGI_FORMAT_R32G32B32A32_TYPELESS:
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
    case DXGI_FORMAT_R32G32B32A32_UINT:
    case DXGI_FORMAT_R32G32B32A32_SINT:
        return 128;

    case DXGI_FORMAT_R32G32B32_TYPELESS:
    case DXGI_FORMAT_R32G32B32_FLOAT:
    case DXGI_FORMAT_R32G32B32_UINT:
    case DXGI_FORMAT_R32G32B32_SINT:
        return 96;

    case DXGI_FORMAT_R16G16B16A16_TYPELESS:
    case DXGI_FORMAT_R16G16B16A16_FLOAT:
    case DXGI_FORMAT_R16G16B16A16_UNORM:
    case DXGI_FORMAT_R16G16B16A16_UINT:
    case DXGI_FORMAT_R16G16B16A16_SNORM:
    case DXGI_FORMAT_R16G16B16A16_SINT:
    case DXGI_FORMAT_R32G32_TYPELESS:
    case DXGI_FORMAT_R32G32_FLOAT:
    case DXGI_FORMAT_R32G32_UINT:
    case DXGI_FORMAT_R32G32_SINT:
    case DXGI_FORMAT_R32G8X24_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
    case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
    case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
        return 64;

    case DXGI_FORMAT_R10G10B10A2_TYPELESS:
    case DXGI_FORMAT_R10G10B10A2_UNORM:
    case DXGI_FORMAT_R10G10B10A2_UINT:
    case DXGI_FORMAT_R11G11B10_FLOAT:
    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_R8G8B8A8_UINT:
    case DXGI_FORMAT_R8G8B8A8_SNORM:
    case DXGI_FORMAT_R8G8B8A8_SINT:
    case DXGI_FORMAT_R16G16_TYPELESS:
    case DXGI_FORMAT_R16G16_FLOAT:
    case DXGI_FORMAT_R16G16_UNORM:
    case DXGI_FORMAT_R16G16_UINT:
    case DXGI_FORMAT_R16G16_SNORM:
    case DXGI_FORMAT_R16G16_SINT:
    case DXGI_FORMAT_R32_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT:
    case DXGI_FORMAT_R32_FLOAT:
    case DXGI_FORMAT_R32_UINT:
    case DXGI_FORMAT_R32_SINT:
    case DXGI_FORMAT_R24G8_TYPELESS:
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
    case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
    case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
    case DXGI_FORMAT_B8G8R8A8_UNORM:
    case DXGI_FORMAT_B8G8R8X8_UNORM:
        return 32;

    case DXGI_FORMAT_R8G8_TYPELESS:
    case DXGI_FORMAT_R8G8_UNORM:
    case DXGI_FORMAT_R8G8_UINT:
    case DXGI_FORMAT_R8G8_SNORM:
    case DXGI_FORMAT_R8G8_SINT:
    case DXGI_FORMAT_R16_TYPELESS:
    case DXGI_FORMAT_R16_FLOAT:
    case DXGI_FORMAT_D16_UNORM:
    case DXGI_FORMAT_R16_UNORM:
    case DXGI_FORMAT_R16_UINT:
    case DXGI_FORMAT_R16_SNORM:
    case DXGI_FORMAT_R16_SINT:
    case DXGI_FORMAT_B5G6R5_UNORM:
    case DXGI_FORMAT_B5G5R5A1_UNORM:
        return 16;

    case DXGI_FORMAT_R8_TYPELESS:
    case DXGI_FORMAT_R8_UNORM:
    case DXGI_FORMAT_R8_UINT:
    case DXGI_FORMAT_R8_SNORM:
    case DXGI_FORMAT_R8_SINT:
    case DXGI_FORMAT_A8_UNORM:
        return 8;

        // Compressed format; http://msdn2.microsoft.com/en-us/library/bb694531(VS.85).aspx
    case DXGI_FORMAT_BC2_TYPELESS:
    case DXGI_FORMAT_BC2_UNORM:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
    case DXGI_FORMAT_BC3_TYPELESS:
    case DXGI_FORMAT_BC3_UNORM:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
    case DXGI_FORMAT_BC5_TYPELESS:
    case DXGI_FORMAT_BC5_UNORM:
    case DXGI_FORMAT_BC5_SNORM:
        return 128;

        // Compressed format; http://msdn2.microsoft.com/en-us/library/bb694531(VS.85).aspx
    case DXGI_FORMAT_R1_UNORM:
    case DXGI_FORMAT_BC1_TYPELESS:
    case DXGI_FORMAT_BC1_UNORM:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
    case DXGI_FORMAT_BC4_TYPELESS:
    case DXGI_FORMAT_BC4_UNORM:
    case DXGI_FORMAT_BC4_SNORM:
        return 64;

        // Compressed format; http://msdn2.microsoft.com/en-us/library/bb694531(VS.85).aspx
    case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
        return 32;

        // These are compressed, but bit-size information is unclear.
    case DXGI_FORMAT_R8G8_B8G8_UNORM:
    case DXGI_FORMAT_G8R8_G8B8_UNORM:
        return 32;

    case DXGI_FORMAT_UNKNOWN:
        break;
    }
    ThrowIfFailed(E_FAIL);
    return 0;
}

eDxShaderConstantType DxHelper::D3D11DimensionToSCT(D3D_SRV_DIMENSION dim)
{
    switch (dim)
    {
    case D3D11_SRV_DIMENSION_BUFFER:
    case D3D11_SRV_DIMENSION_TEXTURE1D:
    case D3D11_SRV_DIMENSION_TEXTURE2D:
    case D3D11_SRV_DIMENSION_TEXTURE2DMS:
        return SCT_TEXTURE;
    case D3D11_SRV_DIMENSION_TEXTURE1DARRAY:
    case D3D11_SRV_DIMENSION_TEXTURE2DARRAY:
    case D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY:
        return SCT_TEXTURE2DARRAY;
    case D3D11_SRV_DIMENSION_TEXTURE3D:
        return SCT_TEXTURE3D;
    case D3D11_SRV_DIMENSION_TEXTURECUBE:
        return SCT_CUBEMAP;
    }
    return SCT_NONE;
}

eDxShaderConstantType DxHelper::D3D11TypeToSCT(const D3D11_SHADER_TYPE_DESC &c)
{
    switch (c.Type)
    {
    case D3D10_SVT_BOOL:
        switch (c.Class)
        {
        case D3D10_SVC_SCALAR:
            return SCT_BOOL;
        case D3D10_SVC_VECTOR:
            if (c.Columns == 2) return SCT_BOOL2;
            if (c.Columns == 3) return SCT_BOOL3;
            return SCT_BOOL4;
        case D3D10_SVC_MATRIX_ROWS: ///< what to do here?
        case D3D10_SVC_MATRIX_COLUMNS:
            return SCT_BOOL4;
        }
        return SCT_BOOL;
    case D3D10_SVT_INT:
        switch (c.Class)
        {
        case D3D10_SVC_SCALAR:
            return SCT_INT;
        case D3D10_SVC_VECTOR:
            if (c.Columns == 2) return SCT_INT2;
            if (c.Columns == 3) return SCT_INT3;
            return SCT_INT4;
        case D3D10_SVC_MATRIX_ROWS: ///< what to do here?
        case D3D10_SVC_MATRIX_COLUMNS:
            return SCT_INT4;
        }
        return SCT_INT;
    case D3D10_SVT_FLOAT:
        switch (c.Class)
        {
        case D3D10_SVC_SCALAR:
            return SCT_FLOAT;
        case D3D10_SVC_VECTOR:
            if (c.Columns == 2) return SCT_FLOAT2;
            if (c.Columns == 3) return SCT_FLOAT3;
            return SCT_FLOAT4;
        case D3D10_SVC_MATRIX_ROWS: ///< what to do here?
        case D3D10_SVC_MATRIX_COLUMNS:
            if (((c.Columns == 3)) && (c.Rows == 4)) return SCT_FLOAT43;
            if ((c.Columns == 3) && (c.Rows == 3)) return SCT_FLOAT33;
            if ((c.Columns == 4) && (c.Rows == 4)) return SCT_FLOAT44;
            return SCT_NONE;
        }
        return SCT_FLOAT;
    case D3D10_SVT_STRING:
        return SCT_STRING;
    case D3D10_SVT_SAMPLER:
        return SCT_SAMPLER;
    case D3D10_SVT_TEXTURE:
    case D3D10_SVT_TEXTURE1D:
    case D3D10_SVT_TEXTURE2D:
        return SCT_TEXTURE;
    case D3D10_SVT_TEXTURE1DARRAY:
    case D3D10_SVT_TEXTURE2DARRAY:
        return SCT_TEXTURE2DARRAY;
    case D3D10_SVT_TEXTURE3D:
        return SCT_TEXTURE3D;
    case D3D10_SVT_TEXTURECUBE:
        return SCT_CUBEMAP;
    }
    return SCT_NONE;
}



DxMarkerScoped::DxMarkerScoped(DxDeviceContext* ctx, const std::wstring& name)
    : context(ctx)
{
    ctx->MarkerBegin(name);
}

DxMarkerScoped::~DxMarkerScoped()
{
    context->MarkerEnd();
}

