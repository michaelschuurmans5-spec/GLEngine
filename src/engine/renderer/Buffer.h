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

class IndexBuffer {
public:
	// TAKES AN ARRAY OF INDICES AND THE TOTAL NUMBER OF INDEX ELEMENTS COUNT 
	IndexBuffer(uint32_t* indices, uint32_t count);
	~IndexBuffer();

	void Bind() const;
	void UnBind() const;

	// GETTER TO KNOW HOW MANY ELEMENTS TO TELL OPENGL TO DRAW LATER 
	uint32_t GetCount() const { return m_Count; }

private:
	uint32_t m_RendererID;
	uint32_t m_Count;

};