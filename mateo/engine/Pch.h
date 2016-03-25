#pragma once

#define NOMINMAX
#include <collection.h>
#include <ppltasks.h>
#include <wrl.h>
#include <wrl/client.h>
#include <dxgi1_4.h>
#include <d3d11_3.h>
#include <d3d11shader.h>
#include <d2d1_3.h>
#include <d2d1effects_2.h>
#include <dwrite_3.h>
#include <wincodec.h>
#include <DirectXColors.h>
#include <DirectXMath.h>
#include <memory>
#include <agile.h>
#include <concrt.h>
#include <array>
#include <concurrent_vector.h>

#include <engine/ResourceId.h>

#include <DirectXTK/Inc/Audio.h>
#include <DirectXTK/Inc/CommonStates.h>
#include <DirectXTK/Inc/DDSTextureLoader.h>
#include <DirectXTK/Inc/DirectXHelpers.h>
#include <DirectXTK/Inc/Effects.h>
#include <DirectXTK/Inc/GamePad.h>
#include <DirectXTK/Inc/GeometricPrimitive.h>
#include <DirectXTK/Inc/GraphicsMemory.h>
#include <DirectXTK/Inc/Keyboard.h>
#include <DirectXTK/Inc/Model.h>
#include <DirectXTK/Inc/Mouse.h>
#include <DirectXTK/Inc/PrimitiveBatch.h>
#include <DirectXTK/Inc/ScreenGrab.h>
#include <DirectXTK/Inc/SimpleMath.h>
#include <DirectXTK/Inc/VertexTypes.h>
#include <DirectXTK/Inc/WICTextureLoader.h>

#define DxSafeRelease(ptr) { if (ptr) { (ptr)->Release(); (ptr)=nullptr; } }
#define SafeDeleteArray(ptr) { if (ptr) { delete [](ptr); (ptr)=nullptr; } }

