#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#if defined(WIN32) || defined(_WIN64)
#include <windows.h>
#endif

#include <vector>
#include <string>
#include <map>
#include <algorithm>

#include <engine/engineinc.h>
// TODO: reference additional headers your program requires here
