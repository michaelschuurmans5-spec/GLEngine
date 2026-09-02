#include "renderer/VertexArray.h"

#include <glad/glad.h>

VertexArray::VertexArray() {
	// GEN A MASTER LAYOUT ARRAY STATE IDENTIFIER ON THE GPU
	glGenVertexArrays(1, &m_RendererID);
}

VertexArray::~VertexArray() {
	glDeleteVertexArrays(1, &m_RendererID);
}

void VertexArray::Bind() const {
	glBindVertexArray(m_RendererID);
}

void VertexArray::Unbind() const {
	glBindVertexArray(0);
}

void VertexArray::AddVertexBuffer(const
	std::shared_ptr<VertexBuffer>& vertexBuffer) {
	// BIND THIS VERTEXARRAY LAYOUT CONTAINER FIRST 
	glBindVertexArray(m_RendererID);

	// BIND THE SOURCE VERTEX BUFFER ASSET TO CONNECT THEM
	vertexBuffer->Bind();

	// FOR BASIC SHAPE , HARDCODE A SIMPLE A COMPONENT POSITION FORMAT X Y Z
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,     // Attribute index location 0 (matches 'layout (location = 0)' in shader)
		3,     // Component count per vertex attribute (X, Y, Z coordinates = 3 floats)
		GL_FLOAT,  // Data type
		GL_FALSE,  //  Normalized flag
		3 * sizeof(float),  //  Stride (Byte size gap between consecutive vertices)
		(void*)0  // Byte offset inside the pointer buffer
	);
}