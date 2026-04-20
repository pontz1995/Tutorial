#pragma once

#include <wrl.h>
#include <d3d12.h>

class Pipeline
{
private:
	Pipeline();

public:
	virtual ~Pipeline();
	static Pipeline* Create(
		ID3DBlob* vsBlob,
		ID3DBlob* psBlob,
		D3D12_INPUT_ELEMENT_DESC* inputLayout,
		size_t numInputLayout
	);

	ID3D12RootSignature* CreateRootSignature();

	ID3D12PipelineState* GetPipelineState() const { return pipelineState.Get(); }
	ID3D12RootSignature* GetRootSignature() const { return rootSig.Get(); }

protected:
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSig;
};