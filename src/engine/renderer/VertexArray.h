#pragma once

#include "renderer/Buffer.h"

#include <memory>
#include <vector>

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
	const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const { return m_VertexBuffers; }

private:
	uint32_t m_RendererID;
	std::shared_ptr<IndexBuffer> m_IndexBuffer;
	// Keeps every vertex buffer this VAO was given alive for as long as the
	// VAO exists. Without this, a caller that passes a temporary/local
	// shared_ptr (e.g. `auto vbo = std::make_shared<VertexBuffer>(...); vao->AddVertexBuffer(vbo);`
	// with no other owner) has that buffer destroyed - and its GL buffer
	// deleted - the moment the local variable goes out of scope, leaving
	// this VAO's glVertexAttribPointer state pointing at a dead GPU buffer.
	std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
};