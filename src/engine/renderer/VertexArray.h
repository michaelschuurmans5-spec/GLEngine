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
	// ATTACH INDEXBUFFER TO THIS VERTEXARRAY LAYOUT CONTAINER
	void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer);
	
	// GETTER PULL OUT THE ATTACHED INDEXBUFFER OBJECT REF
	const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const { return m_IndexBuffer; }

private:
	uint32_t m_RendererID;
	std::shared_ptr<IndexBuffer> m_IndexBuffer;
};