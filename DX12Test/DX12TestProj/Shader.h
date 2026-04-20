#pragma once

#include <wrl.h>
#include <d3d12.h>

class Shader
{

private:

	Shader();

public:
	virtual ~Shader();
	static Shader* Create(const wchar_t* vsPath, const wchar_t* psPath);

	ID3D10Blob* GetVSBlob() const { return vsBlob.Get(); }
	ID3D10Blob* GetPSBlob() const { return psBlob.Get(); }

protected:
	Microsoft::WRL::ComPtr<ID3DBlob> vsBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> psBlob;

};