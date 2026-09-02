#pragma once

#include "renderer/Buffer.h"

#include <memory>

class VertexArray {
public:
	VertexArray();
	~VertexArray();

	void Bind() const;
    void Unbind() const;


	// LINKS A VERTEXBUFFER TO THIS ARRAY AND TELLS OPENGL HOW TO READ ITS ATTRIBUTES
	void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer);

private:
	uint32_t m_RendererID;
};