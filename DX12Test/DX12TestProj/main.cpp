#include <Windows.h>
#include <tchar.h>
#ifdef _DEBUG
#include <iostream>
#endif

#include "main.h"
#include "Scene.h"
using namespace std;

HRESULT InitDX(HWND hwnd);
HRESULT Swapchain(Scene* scene);
#ifdef _DEBUG
void EnableDebugLayer();
#endif

ID3D12Device* _dev = nullptr;
IDXGIFactory6* _dxgiFactory = nullptr;
IDXGISwapChain4* _swapChain = nullptr;
ID3D12CommandAllocator* _cmdAllocator = nullptr;
ID3D12GraphicsCommandList* _cmdList = nullptr;
ID3D12CommandQueue* _cmdQueue = nullptr;
ID3D12DescriptorHeap* _rtvHeaps = nullptr;
ID3D12Fence* _fence = nullptr;
UINT64 _fenceVal = 0;
std::vector<ID3D12Resource*> _backBuffers = {};

// @brief コンソール画面にフォーマット付き文字列を表示
void DebugOutputFormatString(const char* format, ...)
{
#ifdef _DEBUG
	va_list valist;
	va_start(valist, format);
	vprintf(format, valist);
	va_end(valist);
#endif
}

LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (msg == WM_DESTROY)
	{
		PostQuitMessage(0); // OSに対して「もうこのアプリは終わる」と伝える
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

#ifdef _DEBUG
int main()
#else
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
#endif
{
	DebugOutputFormatString("Show Window Test.");
	int c = getchar();

	// ウィンドウクラスの生成＆登録
	WNDCLASSEX w = {};
	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProcedure; // コールバック関数の指定
	w.lpszClassName = _T("DX12Sample"); // アプリケーションクラス名
	w.hInstance = GetModuleHandle(nullptr); // ハンドルの取得

	RegisterClassEx(&w); // アプリケーションクラス（ウィンドウクラスの指定をＯＳに伝える）

	RECT wrc = { 0,0,WINDOW_WIDTH, WINDOW_HEIGHT };
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false); // ウィンドウのサイズを補正する

	// ウィンドウオブジェクトの生成
	HWND hwnd = CreateWindow(
		w.lpszClassName,
		_T("DX12テスト"), // タイトルバーの文字
		WS_OVERLAPPEDWINDOW, // タイトルバーと境界線があるウィンドウ
		CW_USEDEFAULT, // 表示X座標はOSにお任せ
		CW_USEDEFAULT, // 表示Y座標はOSにお任せ
		wrc.right - wrc.left, // ウィンドウ幅
		wrc.bottom - wrc.top, // ウィンドウ高
		nullptr, // 親ウィンドウハンドル
		nullptr, // メニューハンドル
		w.hInstance, // 呼び出しアプリケーションハンドル
		nullptr); // 追加パラメーター
		

#ifdef _DEBUG
	EnableDebugLayer();
#endif
	InitDX(hwnd);

	ShowWindow(hwnd, SW_SHOW); // ウィンドウ表示

	// シーンの生成
	Scene* scene = Scene::Create();

	// メッセージループ
	MSG msg = {};
	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (msg.message == WM_QUIT)
		{
			scene->~Scene();
			break;
		}
		Swapchain(scene);
	}

	UnregisterClass(w.lpszClassName, w.hInstance);
	return 0;
}

HRESULT InitDX(HWND hwnd)
{
	HRESULT result = S_FALSE;
	
	{// デバイス
		D3D_FEATURE_LEVEL levels[] =
		{
			D3D_FEATURE_LEVEL_12_2,
			D3D_FEATURE_LEVEL_12_1,
			D3D_FEATURE_LEVEL_12_0,
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
		};

		D3D_FEATURE_LEVEL featureLevel = levels[0];
		for (auto lv : levels)
		{
			if (D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(&_dev)) == S_OK)
			{
				featureLevel = lv;
				break;
			}
		}
		if (!_dev) return result;

#ifdef _DEBUG
		result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&_dxgiFactory));
#else
		result = CreateDXGIFactory1(IID_PPV_ARGS(&_dxgiFactory));
#endif
		if (result != S_OK) return result;
	}

	{// コマンドキュー
		result = _dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_cmdAllocator)); // コマンドアロケーター生成
		if (result != S_OK) return result;
		result = _dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdAllocator, nullptr, IID_PPV_ARGS(&_cmdList)); // コマンドリスト生成
		if (result != S_OK) return result;
		
		D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
		cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE; // タイムアウトなし
		cmdQueueDesc.NodeMask = 0; // アダプターを１つしか使わないときは０で良い
		cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT; // コマンドリストと合わせる
		result = _dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&_cmdQueue)); // キュー生成
		if (result != S_OK) return result;
	}

	{// スワップチェーン
		DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};
		swapchainDesc.Width = WINDOW_WIDTH;
		swapchainDesc.Height = WINDOW_HEIGHT;
		swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapchainDesc.Stereo = false;
		swapchainDesc.SampleDesc.Count = 1;
		swapchainDesc.SampleDesc.Quality = 0;
		swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
		swapchainDesc.BufferCount = 2; // ダブルバッファ
		swapchainDesc.Scaling = DXGI_SCALING_STRETCH; // バックバッファは伸び縮み可能
		swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // フリップ後は速やかに破棄
		swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED; // 指定なし
		swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // ウィンドウ＜＞フルスクリーン切り替え可能
		result = _dxgiFactory->CreateSwapChainForHwnd(_cmdQueue, hwnd, &swapchainDesc, nullptr, nullptr, (IDXGISwapChain1**)&_swapChain);
	}

	{// レンダーターゲットビュー
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // レンダーターゲットビュー
		heapDesc.NodeMask = 0;
		heapDesc.NumDescriptors = 2; // 表裏の２つ
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE; // 指定なし
		result = _dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&_rtvHeaps));
		if (result != S_OK) return result;

		DXGI_SWAP_CHAIN_DESC swcDesc = {}; // 上で生成したスワップチェーンのディスクリプタ（スコープ分けちゃったんで）
		result = _swapChain->GetDesc(&swcDesc);
		if (result != S_OK) return result;

		_backBuffers = std::vector<ID3D12Resource*>(swcDesc.BufferCount);
		D3D12_CPU_DESCRIPTOR_HANDLE handle = _rtvHeaps->GetCPUDescriptorHandleForHeapStart();
		for (UINT idx = 0; idx < swcDesc.BufferCount; ++idx)
		{
			result = _swapChain->GetBuffer(idx, IID_PPV_ARGS(&_backBuffers[idx]));
			if (result != S_OK) return result;

			_dev->CreateRenderTargetView(_backBuffers[idx], nullptr, handle); // レンダーターゲットビュー生成
			handle.ptr += _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		}
	}

	{// フェンス
		result = _dev->CreateFence(_fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));
	}

	return result;
}

HRESULT Swapchain(Scene* scene)
{
	HRESULT result = S_FALSE;

	UINT bbIdx = _swapChain->GetCurrentBackBufferIndex();

	// リソースバリア
	D3D12_RESOURCE_BARRIER BarrierDesc = {};
	BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	BarrierDesc.Transition.pResource = _backBuffers[bbIdx];
	BarrierDesc.Transition.Subresource = 0;
	BarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	BarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	_cmdList->ResourceBarrier(1, &BarrierDesc);

	auto rtvH = _rtvHeaps->GetCPUDescriptorHandleForHeapStart();
	rtvH.ptr += bbIdx * _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	_cmdList->OMSetRenderTargets(1, &rtvH, true, nullptr); // レンダーターゲットをバックバッファにセット

	float clearColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	_cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr); // 画面クリア

	//シーンの描画
	scene->Render();

	// リソースバリア
	BarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	BarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	_cmdList->ResourceBarrier(1, &BarrierDesc);

	result = _cmdList->Close(); // 命令のクローズ
	ID3D12CommandList* cmdlists[] = { _cmdList };
	_cmdQueue->ExecuteCommandLists(1, cmdlists); // コマンド実行

	_cmdQueue->Signal(_fence, ++_fenceVal);
	if (_fence->GetCompletedValue() != _fenceVal)
	{
		HANDLE event = CreateEvent(nullptr, false, false, nullptr); // イベントハンドルの取得
		if (!event)
		{
			assert(false);
			return S_FALSE;
		}
		else
		{
			_fence->SetEventOnCompletion(_fenceVal, event);
			WaitForSingleObject(event, INFINITE); // イベントが発生するまで待ち続ける
			CloseHandle(event);
		}
	}

	result = _cmdAllocator->Reset(); // キューをクリア
	result = _cmdList->Reset(_cmdAllocator, nullptr); // クローズ状態を解除して再びコマンドリストを溜める準備

	result = _swapChain->Present(1, 0);

	return result;
}

void EnableDebugLayer()
{
	ID3D12Debug* debugLayer = nullptr;
	HRESULT result = D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer));
	debugLayer->EnableDebugLayer();
	debugLayer->Release();
}