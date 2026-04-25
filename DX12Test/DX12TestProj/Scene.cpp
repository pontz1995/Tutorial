#include "main.h"
#include "Scene.h"
#include "Pipeline.h"
#include "Shader.h"
#include "SimplePolygon.h"

Scene* Scene::Create()
{
	Scene* ret = new Scene();
	// ビューポート
	ret->viewport.Width = WINDOW_WIDTH;
	ret->viewport.Height = WINDOW_HEIGHT;
	ret->viewport.TopLeftX = 0;
	ret->viewport.TopLeftY = 0;
	ret->viewport.MinDepth = 0.0f;
	ret->viewport.MaxDepth = 1.0f;

	ret->scissorRect.left = 0;
	ret->scissorRect.top = 0;
	ret->scissorRect.right = ret->scissorRect.left + WINDOW_WIDTH;
	ret->scissorRect.bottom = ret->scissorRect.top + WINDOW_HEIGHT;

	// 描画
	ret->polygon.reset(SimplePolygon::Create());
	ret->shader.reset(Shader::Create(L"BasicVertexShader.hlsl", L"BasicPixelShader.hlsl"));
	ret->pipeline.reset(Pipeline::Create(
		ret->shader->GetVSBlob(),
		ret->shader->GetPSBlob(),
		ret->polygon->GetInputLayout().data(),
		ret->polygon->GetInputLayout().size()
	));

	return ret;
}

void Scene::Update()
{
}

void Scene::Render()
{
	_cmdList->SetPipelineState(pipeline->GetPipelineState());
	_cmdList->SetGraphicsRootSignature(pipeline->GetRootSignature());
	_cmdList->RSSetViewports(1, &viewport);
	_cmdList->RSSetScissorRects(1, &scissorRect);
	_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	_cmdList->IASetVertexBuffers(0, 1, polygon->GetVBView());
	_cmdList->IASetIndexBuffer(polygon->GetIBView());
	_cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);

}

Scene::Scene():
	viewport{},
	scissorRect{},
	polygon(nullptr),
	shader(nullptr),
	pipeline(nullptr)
{
}

Scene::~Scene()
{
	polygon.reset();
	shader.reset();
	pipeline.reset();
}