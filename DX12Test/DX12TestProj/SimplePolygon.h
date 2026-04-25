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


	std::array<D3D12_INPUT_ELEMENT_DESC, 1> GetInputLayout() { return inputLayout; }
	D3D12_VERTEX_BUFFER_VIEW* GetVBView() { return &vbView; }
	D3D12_INDEX_BUFFER_VIEW* GetIBView() { return &ibView; }

protected:

	std::array<D3D12_INPUT_ELEMENT_DESC, 1> inputLayout;
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff;
	Microsoft::WRL::ComPtr<ID3D12Resource> idxBuff;
	D3D12_VERTEX_BUFFER_VIEW vbView;
	D3D12_INDEX_BUFFER_VIEW ibView;

};