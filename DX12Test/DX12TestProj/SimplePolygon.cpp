#include "main.h"
#include "SimplePolygon.h"

using namespace DirectX;
using namespace Microsoft::WRL;

SimplePolygon* SimplePolygon::Create()
{
	SimplePolygon* ret = new SimplePolygon();
	
	//頂点情報
	ret->vertics = std::array<XMFLOAT3, 3>
	{
		XMFLOAT3{-1.0f, -1.0f, 0.0f}, //左下
		XMFLOAT3{-1.0f,  1.0f, 0.0f}, //左上
		XMFLOAT3{ 1.0f, -1.0f, 0.0f}, //右下
	};

	//バッファー
	D3D12_HEAP_PROPERTIES heapprop = {};
	heapprop.Type = D3D12_HEAP_TYPE_UPLOAD; //Map可能
	heapprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapprop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	D3D12_RESOURCE_DESC resdesc = {};
	resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER; //バッファーに使う
	resdesc.Width = sizeof(vertics);
	resdesc.Height = 1; //Widthで表現しているので1で良い
	resdesc.DepthOrArraySize = 1;
	resdesc.MipLevels = 1;
	resdesc.Format = DXGI_FORMAT_UNKNOWN;
	resdesc.SampleDesc.Count = 1; //アンチエイリアス
	resdesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	auto result = _dev->CreateCommittedResource(
		&heapprop,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&ret->vertBuff)
	);
	if (result != S_OK) assert(false);

	//バッファーをマップ
	XMFLOAT3* vertMap = nullptr;
	result = ret->vertBuff->Map(0, nullptr, (void**)&vertMap);
	std::copy(std::begin(ret->vertics), std::end(ret->vertics), vertMap);
	ret->vertBuff->Unmap(0, nullptr);

	//頂点バッファビュー
	ret->vbView.BufferLocation = ret->vertBuff->GetGPUVirtualAddress(); //バッファーの仮想アドレス
	ret->vbView.SizeInBytes = sizeof(ret->vertics);
	ret->vbView.StrideInBytes = sizeof(ret->vertics[0]);

	// 頂点レイアウト
	ret->inputLayout = std::array<D3D12_INPUT_ELEMENT_DESC, 1>
	{
		{ "POSITION", 
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 },
	};


	return ret;
}

SimplePolygon::SimplePolygon():
	vertics(std::array<XMFLOAT3, 3>{}),
	inputLayout(std::array<D3D12_INPUT_ELEMENT_DESC, 1>{}),
	vertBuff(nullptr),
	vbView(D3D12_VERTEX_BUFFER_VIEW{})
{
}

SimplePolygon::~SimplePolygon()
{

}