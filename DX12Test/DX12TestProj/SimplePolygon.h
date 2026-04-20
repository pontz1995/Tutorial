#pragma once

#include <array>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>

class SimplePolygon 
{
private:
	SimplePolygon();

public:
	virtual ~SimplePolygon();
	static SimplePolygon* Create();

	std::array<DirectX::XMFLOAT3, 3> vertics;
	std::array<D3D12_INPUT_ELEMENT_DESC, 1> inputLayout;

	D3D12_VERTEX_BUFFER_VIEW* GetVBView() { return &vbView; }

protected:
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff;
	D3D12_VERTEX_BUFFER_VIEW vbView;


};