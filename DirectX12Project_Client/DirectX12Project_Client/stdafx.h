#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN  

#include <windows.h>

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <iostream>
#include <memory>
#include <string>

#include <vector>
#include <unordered_map>

#include <wrl.h>
#include <shellapi.h>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include "System/ThrowIfFailed.h"
 
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")