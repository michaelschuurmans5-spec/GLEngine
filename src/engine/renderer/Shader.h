#pragma once 

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

class Shader {
public:
	Shader(const std::string& vertexPath, const std::string& fragmentPath);
	~Shader();

	// TELL OPENGL ACTIVATE THIS SHADER PROGRAM FOR DRAW CALLS
	void Bind() const;
	// TELL OPENGL TO DEACTIVATE IT
	void Unbind() const;
	
	// UTILITY FUNCTION CHANGE YOUR FRAGMENT COLOR IN REAL TIME 
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniformMat4(const std::string& name, const glm::mat4& matrix);
	void SetUniformFloat3(const std::string& name, const glm::vec3& vector);
	void SetUniformFloat2(const std::string& name, const glm::vec2& vector);
	void SetUniformInt(const std::string& name, int value);
	void SetUniformFloat(const std::string& name, float value);

	// GETTERS
	uint32_t GetRendererID() const { return m_RendererID; }

private:
	// UNIQUE ID REF GEN BY OPENGL TO IDENTIFY THIS SHADER PROGRAM
	uint32_t m_RendererID;

	// INTERNAL HELPER FUNCTION TO LOAD TEXT FROM A FILE AND COMPILE IT
	std::string ReadFile(const std::string& filepath);
	uint32_t CompileShader(unsigned int type, const std::string& source);
};