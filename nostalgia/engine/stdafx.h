// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#ifndef GY_STDAFX_H
#define GY_STDAFX_H

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#ifdef _DEBUG
#include <thirdparty/vld/include/vld.h>
# pragma comment(lib,"vld.lib")
#endif

// WINDOWS SPECIFIC
#if defined(WIN32) || defined(_WIN64)
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>
#include <xnamath.h>
#include <xinput.h>
#include <dbt.h>
#endif

// C
#include <string.h>
#include <stdio.h>

// STL
#include <vector>
#include <string>
#include <map>
#include <algorithm>

// gy ENGINE
#include <engine/engineinc.h>

#endif