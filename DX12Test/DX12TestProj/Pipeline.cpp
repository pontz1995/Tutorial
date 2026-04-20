#include "main.h"
#include "Shader.h"
#include "Pipeline.h"

using namespace Microsoft::WRL;

Pipeline* Pipeline::Create(
	ID3DBlob* vsBlob,
	ID3DBlob* psBlob,
	D3D12_INPUT_ELEMENT_DESC* inputLayout,
	size_t numInputLayout
)
{
	Pipeline* ret = new Pipeline();

	//グラフィックスパイプラインの流れを設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gPipeline = {};
	gPipeline.VS = { vsBlob->GetBufferPointer(), vsBlob->GetBufferSize() };
	gPipeline.PS = { psBlob->GetBufferPointer(), psBlob->GetBufferSize() };
	gPipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	gPipeline.RasterizerState.MultisampleEnable = false;
	gPipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	gPipeline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	gPipeline.RasterizerState.DepthClipEnable = true;
	gPipeline.BlendState.AlphaToCoverageEnable = false;
	gPipeline.BlendState.IndependentBlendEnable = false;

	D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc = {};
	renderTargetBlendDesc.BlendEnable = false;
	renderTargetBlendDesc.LogicOpEnable = false;
	renderTargetBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	gPipeline.BlendState.RenderTarget[0] = renderTargetBlendDesc;

	gPipeline.InputLayout.pInputElementDescs = inputLayout;
	gPipeline.InputLayout.NumElements = static_cast<UINT>(numInputLayout);
	gPipeline.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	gPipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	gPipeline.NumRenderTargets = 1;
	gPipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	gPipeline.SampleDesc.Count = 1;
	gPipeline.SampleDesc.Quality = 0;

	ret->rootSig = ret->CreateRootSignature();
	gPipeline.pRootSignature = ret->rootSig.Get();

	auto result = _dev->CreateGraphicsPipelineState(&gPipeline, IID_PPV_ARGS(&ret->pipelineState));
	if(result != S_OK)
	{
		assert(false);
		return nullptr;
	}

	return ret;
}

ID3D12RootSignature* Pipeline::CreateRootSignature()
{
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ID3D10Blob* signatureBlob = nullptr;
	ID3D10Blob* errBlob = nullptr;
	auto result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errBlob);
	if (result != S_OK)
	{
		assert(false);
		std::string err;
		err.resize(errBlob->GetBufferSize());
		std::copy_n((char*)errBlob->GetBufferPointer(), errBlob->GetBufferSize(), err.begin());
		OutputDebugStringA(err.c_str());

		return nullptr;
	}

	ID3D12RootSignature* sig = nullptr;
	result = _dev->CreateRootSignature(
		0,
		signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&sig)
	);
	signatureBlob->Release();

	return sig;
}

Pipeline::Pipeline():
	pipelineState(nullptr),
	rootSig(nullptr)
{
}

Pipeline::~Pipeline()
{
}