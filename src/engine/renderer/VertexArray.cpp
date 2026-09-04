#include "renderer/VertexArray.h"
#include "renderer/Buffer.h"

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

	const auto& layout = vertexBuffer->GetLayout();
	uint32_t elementIndex = 0;

	for (const auto& element : layout.GetElements()) {

		// Helper map to convert your enum to active OpenGL types
		GLenum glType = GL_FLOAT;
		int componentCount = 0;
		if (element.Type == ShaderDataType::Float)  componentCount = 1;
		if (element.Type == ShaderDataType::Float2) componentCount = 2;
		if (element.Type == ShaderDataType::Float3) componentCount = 3;
		if (element.Type == ShaderDataType::Float4) componentCount = 4;

		glEnableVertexAttribArray(elementIndex);

		glVertexAttribPointer(
			elementIndex,
			componentCount,
			glType,
			GL_FALSE,
			layout.GetStride(),
			(void*)(uintptr_t)element.Offset
		);
		elementIndex++;
	}

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