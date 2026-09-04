#pragma once 

#include <stdint.h>
#include <string>
#include <vector>

// DEFINE DATA TYPES OF DATA A SHADER CAN ACCEPT
enum class ShaderDataType {
	None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
};
// BYTE SIZES SHADER DATA TYPE
static uint32_t ShaderDataTypeSize(ShaderDataType type) {
	switch (type) {
	case ShaderDataType::Float:   return 4;
	case ShaderDataType::Float2:  return 4 * 2;
	case ShaderDataType::Float3:  return 4 * 3;
	case ShaderDataType::Float4:  return 4 * 4;
	default: return 0;
	}
}
struct BufferElement {
	std::string Name; // shader name
	ShaderDataType Type; // EG: float
	uint32_t Size;  // size
	uint32_t Offset; // address

	BufferElement(ShaderDataType type, const std::string& name)
		: Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0) {
	}
};

class BufferLayout {
public:
	BufferLayout() {}
	BufferLayout(const std::initializer_list<BufferElement>& elements) : m_Elements(elements) {
		CalculateOffsetsAndStride();
	}

	uint32_t GetStride() const { return m_Stride; }
	const std::vector<BufferElement>& GetElements() const { return m_Elements; }

private:
	void CalculateOffsetsAndStride() {
		uint32_t offset = 0;
		m_Stride = 0;
		for (auto& element : m_Elements) {
			element.Offset = offset;
			offset += element.Size;
			m_Stride += element.Size;
		}
	}
	std::vector<BufferElement> m_Elements;
	uint32_t m_Stride = 0;
};

class VertexBuffer {
public:
	VertexBuffer(float* vertices, uint32_t size);
	~VertexBuffer();

	void Bind() const;
	void UnBind() const;

	// GETTERS
	void SetLayout(const BufferLayout& layout) { m_Layout = layout; }
	const BufferLayout& GetLayout() const { return m_Layout; }


private:
	uint32_t m_RendererID;
	BufferLayout m_Layout;
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