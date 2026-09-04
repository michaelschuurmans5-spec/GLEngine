#include "renderer/Shader.h"
#include "core/Log.h"
#include <glad/glad.h>

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <glm/gtc/type_ptr.hpp>


Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) {
	// READ FILES FROM DISK INTO MEMORY STRINGS
	std::string vertexSource = ReadFile(vertexPath);
	std::string fragmentSource = ReadFile(fragmentPath);

	if (vertexSource.empty() || fragmentSource.empty()) {
		ENGINE_ERROR("Critical Error: Shader source files are empty or unreadable!");
		return;
	}

	// CREATE PROGRAM CONTAINER AND COMPILE COMPONENTS
	m_RendererID = glCreateProgram();
	// COMPLIE THE INDIVIDUAL VERTEX AND FRAGMENT CODES 
	uint32_t vs = CompileShader(GL_VERTEX_SHADER, vertexSource);
	uint32_t fs = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);


	// ATTACH BOTH PROGRAMS TO OUR MASTER PROGRAM CONTAINER 
	glAttachShader(m_RendererID, vs);
	glAttachShader(m_RendererID, fs);
	// LINK THEM TOGETHER INTO A SINGLE EXECUTABLE PIPELINE ON THE GPU 
	glLinkProgram(m_RendererID);

	// CHECK FOR LINKING ERRORS 
	int isLinked = 0;
	glGetProgramiv(m_RendererID, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE) {
		int maxLength = 0;
		glGetProgramiv(m_RendererID, GL_INFO_LOG_LENGTH, &maxLength);

		if (maxLength > 0) {
			std::vector<char> infoLog(maxLength);
			glGetProgramInfoLog(m_RendererID, maxLength, &maxLength, infoLog.data());
			ENGINE_ERROR("Shader linking failed:" + std::string(infoLog.data()));
		}
		else {
			ENGINE_ERROR("Shader linking failed with an unmapped context error.");
		}

		glDeleteProgram(m_RendererID);
		glDeleteShader(vs);
		glDeleteShader(fs);
		return;
	}

	glDetachShader(m_RendererID, vs);
	glDetachShader(m_RendererID, fs);
	glDeleteShader(vs);
	glDeleteShader(fs);

	ENGINE_INFO("Shader complied and linked successfully!");
}

Shader::~Shader() {
	// CLEANUP THE GPU ASSET WHEN THE OBJECT DIES 
	glDeleteProgram(m_RendererID);
}

void Shader::Bind() const {
	glUseProgram(m_RendererID);
}

void Shader::Unbind() const {
	glUseProgram(0);
}

// UPLOADS 4 COLOR FLOAT VECTORS DYNAMICALLY TO YOUR SHADER PROGRAM UNIFORM SLOT
void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3) {
	int location = glGetUniformLocation(m_RendererID, name.c_str());
	if (location == -1) {
		ENGINE_WARN("Warning: Uniform '" + name + "' not found in shader source!");
		return;
	}
	glUniform4f(location, v0, v1, v2, v3);
}
// OPENS FILE PATHWAY STREAM & CONVERTS ITS CHARACTERS INTO CLEAN C++ STRING
std::string Shader::ReadFile(const std::string& filepath) {
	std::string result;
	std::ifstream in(filepath, std::ios::in | std::ios::binary); 
	if (in) {
		std::stringstream ss;
		ss << in.rdbuf(); 
		result = ss.str();
		in.close();
	}
	else {
		ENGINE_ERROR("Could not open file stream path: " + filepath);
	}
	return result;
}
void Shader::SetUniformMat4(const std::string& name, const glm::mat4& matrix)
{
	int location = glGetUniformLocation(m_RendererID, name.c_str());
	if (location == -1) {
		ENGINE_WARN("Warning: Uniform '" + name + "' not found in shader source!");
		return;
	}
	
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}
void Shader::SetUniformFloat3(const std::string& name, const glm::vec3& vector) {
	int location = glGetUniformLocation(m_RendererID, name.c_str());
	if (location == -1) {
		// Keeps track of unused or optimized out lighting variables
		ENGINE_WARN("Warning: Uniform '" + name + "' not found in shader source!");
		return;
	}
	glUniform3fv(location, 1, glm::value_ptr(vector));
}
void Shader::SetUniformFloat2(const std::string& name, const glm::vec2& vector) {
	int location = glGetUniformLocation(m_RendererID, name.c_str());
	if (location == -1) {
		ENGINE_WARN("Warning: Uniform '" + name + "' not found in shader source!");
		return;
	}
	glUniform2fv(location, 1, glm::value_ptr(vector));
}
uint32_t Shader::CompileShader(unsigned int type,
	const std::string& source) {
	// GENERATE TEMPORARY SLOT FOR THIS SHADER STAGE
	uint32_t id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	// ERROR CHECKING
	int isCompiled = 0;
	glGetShaderiv(id, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE) {
		int maxLength = 0;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);

		if (maxLength > 0) {
			std::vector<char> infoLog(maxLength);
			glGetShaderInfoLog(id, maxLength, &maxLength, infoLog.data());

			std::string shaderTypeStr = (type == GL_VERTEX_SHADER) ? "Vertex" : "Fragment";
			ENGINE_ERROR(shaderTypeStr + " shader compilation failed: " + std::string(infoLog.data()));
		}
		glDeleteShader(id);
		return 0;
	}

	return id;
}