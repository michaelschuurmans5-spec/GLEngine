#include "renderer/VertexArray.h"

#include <glad/glad.h>

// VERTEX ARRAY
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
	// BIND THIS VERTEX ARRAY LAYOUT CONTAINER FIRST 
	glBindVertexArray(m_RendererID); // BIND TO ID: inside GPU driver
	// BIND THE SOURCE VERTEX BUFFER ASSET TO CONNECT THEM: translates data
	vertexBuffer->Bind();

	// TOTAL LAYOUT  STRIDE GAP = 5 FLOATS : 3 positions & 2 for UVs
	uint32_t strideSize = 5 * sizeof(float);

	// POSITION ATTRIBUTE (Slot location index 0 - vec3)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,     // Attribute index location 0 (matches 'layout (location = 0)' in shader)
		3,     // Component count per vertex attribute (X, Y, Z coordinates = 3 floats)
		GL_FLOAT,  // Data type
		GL_FALSE,  //  Normalized flag
		strideSize, //  jump gap to reach the next vertex coordinate (5 floats)
		(void*)0  // Starts right at byte location zero
	);

	// TEXTURE COORD ATTRIBUTE (Slot location index 1 - vec2)
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,
		2, // U, V coordinates
		GL_FLOAT,
		GL_FALSE,
		strideSize,
		(void*)(3 * sizeof(float)) // NEW STRIDE OFFSET: Skips the 3 position floats to find UV bytes!
	);
}
// LINK INDEX BUFFER OBJECT TO THIS VERTEX ARRAY STATE MAP
void VertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) {
	// BIND VERTEX ARRAY CONTAINER FIRST TO MAKE IT ACTIVE 
	glBindVertexArray(m_RendererID);
	// BIND INDEX BUFFER HARDWARE ASSET TO LINK THEM TOGETHER 
	indexBuffer->Bind();
	// KEEP TRACK OF IT INSIDE OUR CLASS VARIABLE SLOT 
	m_IndexBuffer = indexBuffer;
}