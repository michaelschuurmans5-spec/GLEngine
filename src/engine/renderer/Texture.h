#pragma once

#include <string>
#include <cstdint>

class Texture {
public:
	Texture(const std::string& path);
	~Texture();

	// TEXTURES BIND TO HARDWARE SLOTS 
	void Bind(uint32_t slot = 0) const;
	void UnBind() const;

private:
	uint32_t m_RendererID;  
	std::string m_FilePath;
	int m_Width, m_Height, m_BPP; // BITS PER PIXEL TRACKING FIELDS 
};