#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <memory>

class SimplePolygon;
class Shader;
class Pipeline;
class Scene
{
private:
	Scene();

public:
	virtual ~Scene();
	static Scene* Create();
	virtual void Update();
	virtual void Render();

protected:
	D3D12_VIEWPORT viewport;
	D3D12_RECT scissorRect;
	std::unique_ptr<SimplePolygon> polygon;
	std::unique_ptr<Shader> shader;
	std::unique_ptr<Pipeline> pipeline;

};