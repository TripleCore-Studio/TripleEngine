#include "ShaderProgram.h"
#include <glad/glad.h>

namespace TripleEngineCore {
	namespace TripleRenderer {
		ShaderProgram::ShaderProgram()
		{
			_compiled = false;
			_programID = 0;
		}
		bool ShaderProgram::compileProgram(const std::string& verSource, const std::string& fragSource)
		{
			if (!this->isCompiled()) {
				unsigned int vertexShader = compileShader(verSource, GL_VERTEX_SHADER, this->_errorLog);
				if (vertexShader == 0) {
					return false;
				}
				unsigned int fragmentShader = compileShader(fragSource, GL_FRAGMENT_SHADER, this->_errorLog);
				if (fragmentShader == 0) {
					glDeleteShader(vertexShader);
					return false;
				}
				_programID = linkProgram(vertexShader, fragmentShader, this->_errorLog);
				if (_programID == 0) {
					glDeleteShader(vertexShader);
					glDeleteShader(fragmentShader);
					return false;
				}
				glDeleteShader(vertexShader);
				glDeleteShader(fragmentShader);
				this->_compiled = true;
				return true;
			}
			else {
				_errorLog = "Shader program already compiled.";
				return false;
			}
		}
		void ShaderProgram::use()
		{
			if (this->_compiled && this->_programID != 0) {
				glUseProgram(this->_programID);
			}
		}
		void ShaderProgram::setUniformMat4(const std::string& name, const float* matrix)
		{
			if (this->_compiled && this->_programID != 0) {
				GLint location = glGetUniformLocation(this->_programID, name.c_str());
				if (location != -1) {
					glUniformMatrix4fv(location, 1, GL_FALSE, matrix);
				}
			}
		}
		bool ShaderProgram::deleteProgram()
		{
			if (_programID != 0) {
				glDeleteProgram(_programID);
				_programID = 0;
				_compiled = false;
				_errorLog.clear();
				return true;
			}
			else {
				_errorLog = "Shader program not initialized or already deleted.";
				return false;
			}
		}
		ShaderProgram::~ShaderProgram()
		{
			if (_programID != 0) {
				glDeleteProgram(_programID);
			}
			_programID = 0;
			_compiled = false;
			_errorLog.clear();
		}
		unsigned int ShaderProgram::compileShader(const std::string& source, unsigned int shaderType, std::string& outLog)
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
		unsigned int ShaderProgram::linkProgram(unsigned int vertexShader, unsigned int fragmentShader, std::string& outLog)
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
		bool ShaderProgram::operator==(const ShaderProgram& other) const
		{
			return _programID == other._programID &&
				_compiled == other._compiled &&
				_errorLog == other._errorLog;
		}
		bool ShaderProgram::operator!=(const ShaderProgram& other) const
		{
			return !(*this == other);
		}
		ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept
			: _programID(other._programID),
			_compiled(other._compiled),
			_errorLog(std::move(other._errorLog))
		{
			other._programID = 0;
			other._compiled = false;
		}
		ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) noexcept
		{
			if (this != &other) {
				if (_programID != 0) {
					glDeleteProgram(_programID);
				}

				_programID = other._programID;
				_compiled = other._compiled;
				_errorLog = std::move(other._errorLog);

				other._programID = 0;
				other._compiled = false;
			}
			return *this;
		}
	}
}
