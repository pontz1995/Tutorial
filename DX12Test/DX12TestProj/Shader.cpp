#include "main.h"
#include "Shader.h"

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

using namespace Microsoft::WRL;

Shader* Shader::Create(const wchar_t* vsPath, const wchar_t* psPath)
{
	ID3DBlob* errBlob = nullptr;

	Shader* ret = new Shader();

	//頂点シェーダー
	auto result = D3DCompileFromFile(
		vsPath,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"BasicVS",
		"vs_5_1",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&ret->vsBlob,
		&errBlob
	);
	if (result != S_OK)
	{
		assert(false);
		if(result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
		{
			OutputDebugStringA("シェーダーファイルが見つかりませんでした");
			return nullptr;
		}
		std::string err;
		err.resize(errBlob->GetBufferSize());
		std::copy_n((char*)errBlob->GetBufferPointer(), errBlob->GetBufferSize(), err.begin());
		OutputDebugStringA(err.c_str());

		return nullptr;
	}

	//ピクセルシェーダー
	result = D3DCompileFromFile(
		psPath,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"BasicPS",
		"ps_5_1",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&ret->psBlob,
		&errBlob
	);
	if (result != S_OK)
	{
		assert(false);
		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
		{
			OutputDebugStringA("シェーダーファイルが見つかりませんでした");
			return nullptr;
		}
		std::string err;
		err.resize(errBlob->GetBufferSize());
		std::copy_n((char*)errBlob->GetBufferPointer(), errBlob->GetBufferSize(), err.begin());
		OutputDebugStringA(err.c_str());

		return nullptr;
	}

	return ret;
}

Shader::Shader():
	vsBlob(nullptr),
	psBlob(nullptr)
{
}

Shader::~Shader()
{
}