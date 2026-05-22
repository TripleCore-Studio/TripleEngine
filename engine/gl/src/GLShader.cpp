#include "triple/gl/GLShader.h"
#include <glad/glad.h>

namespace triple::gl {
	GLShader::GLShader()
	{
		m_compiled = false;
		m_programID = 0;
	}
	bool GLShader::compileProgram(const std::string& verSource, const std::string& fragSource)
	{
		if (!this->isCompiled()) {
			unsigned int vertexShader = compileShader(verSource, GL_VERTEX_SHADER, this->m_errorLog);
			if (vertexShader == 0) {
				return false;
			}
			unsigned int fragmentShader = compileShader(fragSource, GL_FRAGMENT_SHADER, this->m_errorLog);
			if (fragmentShader == 0) {
				glDeleteShader(vertexShader);
				return false;
			}
			m_programID = linkProgram(vertexShader, fragmentShader, this->m_errorLog);
			if (m_programID == 0) {
				glDeleteShader(vertexShader);
				glDeleteShader(fragmentShader);
				return false;
			}
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);
			this->m_compiled = true;
			return true;
		}
		else {
			m_errorLog = "Shader program already compiled.";
			return false;
		}
	}
	void GLShader::bind()
	{
		if (this->m_compiled && this->m_programID != 0) {
			glUseProgram(this->m_programID);
		}
	}
	void GLShader::setUniformMat4(const std::string& name, const float* matrix)
	{
		if (this->m_compiled && this->m_programID != 0) {
			GLint location = glGetUniformLocation(this->m_programID, name.c_str());
			if (location != -1) {
				glUniformMatrix4fv(location, 1, GL_FALSE, matrix);
			}
		}
	}
	void GLShader::setUniform1f(const std::string& name, float f)
	{
		if (this->m_compiled && this->m_programID != 0) {
			GLint location = glGetUniformLocation(this->m_programID, name.c_str());
			if (location != -1) {
				glUniform1f(location, f);
			}
		}
	}
	void GLShader::setUniform3fv(const std::string& name, const float* vec3) {
		if (m_compiled && m_programID != 0) {
			GLint loc = glGetUniformLocation(m_programID, name.c_str());
			if (loc != -1) {
				glUniform3fv(loc, 1, vec3);
			}
		}
	}
	void GLShader::setUniform1i(const std::string& name, int value) {
		if (m_compiled && m_programID != 0) {
			GLint loc = glGetUniformLocation(m_programID, name.c_str());
			if (loc != -1) {
				glUniform1i(loc, value);
			}
		}
	}
	void GLShader::setTexture(const std::string& name, unsigned int textureID, int slot)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, textureID);
		setUniform1i(name, slot);
	}
	bool GLShader::deleteProgram()
	{
		if (m_programID != 0) {
			glDeleteProgram(m_programID);
			m_programID = 0;
			m_compiled = false;
			m_errorLog.clear();
			return true;
		}
		else {
			m_errorLog = "Shader program not initialized or already deleted.";
			return false;
		}
	}
	GLShader::~GLShader()
	{
		if (m_programID != 0) {
			glDeleteProgram(m_programID);
		}
		m_programID = 0;
		m_compiled = false;
		m_errorLog.clear();
	}
	unsigned int GLShader::compileShader(const std::string& source, unsigned int shaderType, std::string& outLog)
	{
		GLuint id = glCreateShader(shaderType);
		const char* verSourcePtr = source.c_str();
		glShaderSource(id, 1, &verSourcePtr, nullptr);
		glCompileShader(id);

		GLint success;
		glGetShaderiv(id, GL_COMPILE_STATUS, &success);
		if (!success) {
			GLint logLength;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLength);
			if (logLength > 0) {
				std::string log(logLength, '\0');
				glGetShaderInfoLog(id, logLength, nullptr, log.data());
				outLog = std::move(log);
			}
			else {
				outLog.clear();
			}
			glDeleteShader(id);
			return 0; // Compilation failed
		}
		outLog.clear();
		return id; // Compilation succeeded
	}
	unsigned int GLShader::linkProgram(unsigned int vertexShader, unsigned int fragmentShader, std::string& outLog)
	{
		unsigned int id = glCreateProgram();
		glAttachShader(id, vertexShader);
		glAttachShader(id, fragmentShader);
		glLinkProgram(id);
		GLint success;
		glGetProgramiv(id, GL_LINK_STATUS, &success);
		if (!success) {
			GLint logLength;
			glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logLength);
			if (logLength > 0) {
				std::string log(logLength, '\0');
				glGetProgramInfoLog(id, logLength, nullptr, log.data());
				outLog = std::move(log);
			}
			else {
				outLog.clear();
			}
			glDeleteProgram(id);
			return 0;
		}
		outLog.clear();
		return id;
	}
	bool GLShader::operator==(const GLShader& other) const
	{
		return m_programID == other.m_programID &&
			m_compiled == other.m_compiled &&
			m_errorLog == other.m_errorLog;
	}
	bool GLShader::operator!=(const GLShader& other) const
	{
		return !(*this == other);
	}
	GLShader::GLShader(GLShader&& other) noexcept
		: m_programID(other.m_programID),
		m_compiled(other.m_compiled),
		m_errorLog(std::move(other.m_errorLog))
	{
		other.m_programID = 0;
		other.m_compiled = false;
	}
	GLShader& GLShader::operator=(GLShader&& other) noexcept
	{
		if (this != &other) {
			if (m_programID != 0) {
				glDeleteProgram(m_programID);
			}

			m_programID = other.m_programID;
			m_compiled = other.m_compiled;
			m_errorLog = std::move(other.m_errorLog);

			other.m_programID = 0;
			other.m_compiled = false;
		}
		return *this;
	}
}
