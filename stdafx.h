// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#undef min
#undef max
#undef CreateWindow

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <io.h> // Need this for console
#include <cassert>
#include <vector>
#include <cmath>
#include <cfloat>
#include <algorithm>
#include <memory>
#include <string>

#ifdef INFINITY
#undef INFINITY
#endif

const float INFINITY = FLT_MAX;
const float PI = 3.14159265358979323846f;
const float ONE_OVER_PI = 1.0f / PI;

inline float Deg2Rad(float deg) { return deg / 180.0f * PI; }

inline float Log2(float x) {
	static const float invLog2 = 1.f / logf(2.f);
	return logf(x) * invLog2;
}
inline int Log2Int(float v) {
	static const float doublemagicroundeps = (0.5 - 1.4e-11);
	return int(Log2(v) + doublemagicroundeps);
}

typedef unsigned char Byte;

#include "config.h"
#include "Vec3.h"
#include "Matrix.h"
