#include "Texture.h"
#include "core/Log.h"

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"


Texture::Texture(const std::string& path) 
	:m_RendererID(0), m_FilePath(path), m_Width(0), m_Height(0), m_BPP(0)
{
	// ENABLE VERTICAL FLIPPING BEFORE LOADING AN IMAGE/TEXTURE
	stbi_set_flip_vertically_on_load(1);

	// CONVERT TO C++ STRING TO C STYLE SO STB CAN READ IT
	unsigned char* localBuffer = stbi_load(path.c_str(),
		// TAKE VARIABLES OVERWRITE VALUES REQUEST FOUR CHANNELS RGBA
		&m_Width, &m_Height, &m_BPP, 4); 

		// HOLDS ARRAY PIXEL DATA 
		if (!localBuffer) {
			ENGINE_ERROR("Texture failed to load at path:" + path);
			return;
		}

	// GENERATE AN IDENTITY TAG HOLDER TAKEN ON THE GPU
	glGenTextures(1, &m_RendererID);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);

	// CONFIGURE HARDWARE FILTERING : how to handle scaling textures up or down
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // smooth 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // clip werid border artifacts
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// UPLOAD THE DATA: Copy raw buffer channels directly up onto your VRAM
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
		m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer);
	glGenerateMipmap(GL_TEXTURE_2D);

	// CLEANUP CPU BUFFER : Once data is on your graphics card, free up system RAM instantly
	stbi_image_free(localBuffer);
	glBindTexture(GL_TEXTURE_2D, 0);

	ENGINE_INFO("Texture loaded successfully: " + path + 
		" (" + std::to_string(m_Width) + "x" + std::to_string(m_Height) + ")");
}

Texture::~Texture() {
	glDeleteTextures(1, &m_RendererID);
}

void Texture::Bind(uint32_t slot) const {
	// ACTIVATE REQUEST TEXTURE SLOT CONTAINER LANES 
	glActiveTexture(GL_TEXTURE0 + slot);
	// BIND SLOT WITH ID 
	glBindTexture(GL_TEXTURE_2D, m_RendererID);
}

void Texture::UnBind() const {
	// BIND NULL ID
	glBindTexture(GL_TEXTURE_2D, 0);
}