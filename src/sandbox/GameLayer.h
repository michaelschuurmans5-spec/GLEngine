#pragma once 

#include "core/Layer.h"
#include "renderer/shader.h"
#include "renderer/VertexArray.h"
#include "renderer/Buffer.h"    

#include <memory>

class GameLayer : public Layer {
public:
	GameLayer();
	virtual ~GameLayer() = default;

	// OVERRIDING THE LIFECYCLE HOOKS FROM YOUR BASE LAYER CLASS
	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate() override;
	virtual void OnRender() override;
private:
	// SHADER
	std::unique_ptr<Shader> m_Shader;

	// ARRAY & BUFFER
	std::shared_ptr<VertexArray> m_VertexArray;
	std::shared_ptr<VertexBuffer> m_VertexBuffer;
};