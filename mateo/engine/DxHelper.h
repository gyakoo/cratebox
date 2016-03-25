#pragma once

#include <ppltasks.h>	// For create_task
#include <engine/DxTypes.h>

namespace Engine
{
    // forward
    class DxDeviceContext;

	inline void ThrowIfFailed(HRESULT hr, Platform::String^ msg)
	{
		if (FAILED(hr))
		{
			// Set a breakpoint on this line to catch Win32 API errors.
			throw Platform::Exception::CreateException(hr, msg);
		}
	}

	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			// Set a breakpoint on this line to catch Win32 API errors.
			throw Platform::Exception::CreateException(hr);
		}
	}

    inline void ThrowIfAssertAlways(bool exp, const wchar_t* msg = nullptr)
    {
        if (msg)
            ThrowIfFailed(exp ? S_OK : E_FAIL, ref new	Platform::String(msg));
        else
            ThrowIfFailed(exp ? S_OK : E_FAIL);
    }

    struct DxMarkerScoped
    {
        DxMarkerScoped(DxDeviceContext* ctx, const std::wstring& name);
        ~DxMarkerScoped();

        DxDeviceContext* context;
    };

#if defined(_DEBUG) || defined(DEBUG)
#   define ThrowIfAssert(...) ThrowIfAssertAlways(__VA_ARGS__)
#   define DX_MARKER_SCOPED(context, name) DxMarkerScoped(context, name)
#else
#   define ThrowIfAssert(...)
#   define DX_MARKER_SCOPED(context, name)
#endif

#define ThrowNotImplemented() ThrowIfAssertAlways(false, L"Not Implemented");

	// Function that reads from a binary file asynchronously.
	inline Concurrency::task<std::vector<byte>> ReadDataAsync(const std::wstring& filename)
	{
		using namespace Windows::Storage;
		using namespace Concurrency;

		auto folder = Windows::ApplicationModel::Package::Current->InstalledLocation;

		return create_task(folder->GetFileAsync(Platform::StringReference(filename.c_str()))).then([] (StorageFile^ file) 
		{
			return FileIO::ReadBufferAsync(file);
		}).then([] (Streams::IBuffer^ fileBuffer) -> std::vector<byte> 
		{
			std::vector<byte> returnBuffer;
			returnBuffer.resize(fileBuffer->Length);
			Streams::DataReader::FromBuffer(fileBuffer)->ReadBytes(Platform::ArrayReference<byte>(returnBuffer.data(), fileBuffer->Length));
			return returnBuffer;
		});
	}
    
	// Converts a length in device-independent pixels (DIPs) to a length in physical pixels.
	inline float ConvertDipsToPixels(float dips, float dpi)
	{
		static const float dipsPerInch = 96.0f;
		return floorf(dips * dpi / dipsPerInch + 0.5f); // Round to nearest integer.
	}


    namespace DxHelper
    {
#if defined(_DEBUG)
        // Check for SDK Layer support.
        inline bool SdkLayersAvailable()
        {
            HRESULT hr = D3D11CreateDevice(
                nullptr,
                D3D_DRIVER_TYPE_NULL,       // There is no need to create a real hardware device.
                0,
                D3D11_CREATE_DEVICE_DEBUG,  // Check for the SDK layers.
                nullptr,                    // Any feature level will do.
                0,
                D3D11_SDK_VERSION,          // Always set this to D3D11_SDK_VERSION for Windows Store apps.
                nullptr,                    // No need to keep the D3D device reference.
                nullptr,                    // No need to know the feature level.
                nullptr                     // No need to keep the D3D device context reference.
                );

            return SUCCEEDED(hr);
        }
#endif
        int32_t CreateEmptyTexture2D(int32_t width, int32_t height, DXGI_FORMAT texf, uint32_t bindflags, ID3D11Texture2D** pOutTex);
        int32_t SizeOfFormatElement(DXGI_FORMAT format);
        eDxShaderConstantType D3D11DimensionToSCT(D3D_SRV_DIMENSION dim);
        eDxShaderConstantType D3D11TypeToSCT(const D3D11_SHADER_TYPE_DESC &c);
    };
}
