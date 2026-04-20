#pragma once

#include <assert.h>

#include <d3d12.h>
#include <dxgi1_6.h>
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#include <vector>
#include <string>

#define WINDOW_WIDTH 720
#define WINDOW_HEIGHT 480

extern ID3D12Device* _dev;
extern ID3D12GraphicsCommandList* _cmdList;

void DebugOutputFormatString(const char* format, ...);