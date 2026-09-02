#include "renderer/Buffer.h"

#include <glad/glad.h>

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