#include "main.h"
#include "SimplePolygon.h"

using namespace DirectX;
using namespace Microsoft::WRL;

SimplePolygon* SimplePolygon::Create()
{
	SimplePolygon* ret = new SimplePolygon();
	
	//頂点情報
	std::array<XMFLOAT3, 4> vertics =
	{
		XMFLOAT3{-0.4f, -0.7f, 0.0f}, //左下
		XMFLOAT3{-0.4f,  0.7f, 0.0f}, //左上
		XMFLOAT3{ 0.4f, -0.7f, 0.0f}, //右下
		XMFLOAT3{ 0.4f,  0.7f, 0.0f}, //右上
	};

	//バッファー
	D3D12_HEAP_PROPERTIES heapprop = {};
	heapprop.Type = D3D12_HEAP_TYPE_UPLOAD; //Map可能
	heapprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapprop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	//頂点バッファー
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

	// 転送用リソースの作成
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
	std::copy(std::begin(vertics), std::end(vertics), vertMap);
	ret->vertBuff->Unmap(0, nullptr);
	if (vertMap) vertMap = nullptr;

	//頂点バッファビュー
	ret->vbView.BufferLocation = ret->vertBuff->GetGPUVirtualAddress(); //バッファーの仮想アドレス
	ret->vbView.SizeInBytes = sizeof(vertics);
	ret->vbView.StrideInBytes = sizeof(vertics[0]);

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


	// インデックスバッファー
	unsigned short indicies[] = { 
		0, 1, 2,
		2, 1, 3 
	};

	ret->idxBuff = nullptr;
	resdesc.Width = sizeof(indicies); //バッファーのサイズ以外、頂点バッファーの設定を使いまわしてよい
	
	// 転送用リソースの作成
	result = _dev->CreateCommittedResource(
		&heapprop,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&ret->idxBuff)
	);

	//バッファーをマップ
	unsigned short* idxMap = nullptr;
	result = ret->idxBuff->Map(0, nullptr, (void**)&idxMap);
	std::copy(std::begin(indicies), std::end(indicies), idxMap);
	ret->idxBuff->Unmap(0, nullptr);
	if (idxMap) idxMap = nullptr;

	// インデックスバッファビュー
	ret->ibView = {};
	ret->ibView.BufferLocation = ret->idxBuff->GetGPUVirtualAddress();
	ret->ibView.Format = DXGI_FORMAT_R16_UINT;
	ret->ibView.SizeInBytes = sizeof(indicies);

	return ret;
}

SimplePolygon::SimplePolygon():
	inputLayout(std::array<D3D12_INPUT_ELEMENT_DESC, 1>{}),
	vertBuff(nullptr),
	idxBuff(nullptr),
	vbView(D3D12_VERTEX_BUFFER_VIEW{}),
	ibView(D3D12_INDEX_BUFFER_VIEW{})
{
}

SimplePolygon::~SimplePolygon()
{
	vertBuff.Reset();
	idxBuff.Reset();
}