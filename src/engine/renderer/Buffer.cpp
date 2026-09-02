#include "renderer/Buffer.h"

#include <glad/glad.h>

// VERTEX BUFFER  
VertexBuffer::VertexBuffer(float* vertices, uint32_t size) {
	// GEN 1 MEMORY BUFFER IDENIFIER  TAG ON GPU 
	glGenBuffers(1, &m_RendererID);
    // BIND IT SO OPENGL KNOWS SUBSEQUENT BUFFER CALLS CALLS APPLY TO THIS OBJECT 
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	// UPLOAD THE RAW FLOAT ARRAY SIZE STREAMS DIRECTLY TO THE GPU
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}
VertexBuffer::~VertexBuffer() {
	// CLEANUP OUR SPECIFIC GPU ALLOCATION WHEN THIS OBJECT GOES OUF OF SCOPE
	glDeleteBuffers(1, &m_RendererID);
}
void VertexBuffer::Bind() const {
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
}
void VertexBuffer::UnBind() const {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
// INDEX BUFFER 
IndexBuffer::IndexBuffer(uint32_t* indices, uint32_t count) : m_Count(count) {
	// GENERATE 1 BUFFER IDENTIFER TAG ON THE GPU
	glGenBuffers(1, &m_RendererID);
	// BIND TO GL ELEMENT ARRAY BUFFER INSTEAD OF GL ARRAY BUFFER 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	// UPLOAD RAW INTEGER ARRAY TO GPU MEMORY
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
}
IndexBuffer::~IndexBuffer() {
	glDeleteBuffers(1, &m_RendererID);
}
void IndexBuffer::Bind() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
}
void IndexBuffer::UnBind() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
