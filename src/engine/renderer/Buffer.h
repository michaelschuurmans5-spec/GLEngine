#pragma once 

#include <stdint.h>

class VertexBuffer {
public:
	VertexBuffer(float* vertices, uint32_t size);
	~VertexBuffer();

	void Bind() const;
	void UnBind() const;

private:
	uint32_t m_RendererID;

};