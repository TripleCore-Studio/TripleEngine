#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include <string>

namespace TripleRenderer::GLRenderer::Resources {
	struct GLShader {
	public:
		GLShader();
		bool compileProgram(const std::string& verSource, const std::string& fragSource);
		void bind();

		void setUniformMat4(const std::string& name, const float* matrix);
		void setUniform1f(const std::string& name, float f);
		void setUniform3fv(const std::string& name, const float* vec3);
		void setUniform1i(const std::string& name, int value);

		void setTexture(const std::string& name, unsigned int textureID, int slot);

		bool isCompiled() const { return _compiled; }
		std::string getErrorLog() const { return _errorLog; }
		unsigned int getProgramID() const { return _programID; }
		bool deleteProgram();
		~GLShader();

		bool operator==(const GLShader& other) const;
		bool operator!=(const GLShader& other) const;

		GLShader(GLShader&& other) noexcept;
		GLShader& operator=(GLShader&& other) noexcept;
	private:
		unsigned int _programID;
		bool _compiled;
		std::string _errorLog;
		unsigned int compileShader(const std::string& source, unsigned int shaderType, std::string& outLog);
		unsigned int linkProgram(unsigned int vertexShader, unsigned int fragmentShader, std::string& outLog);

		GLShader(const GLShader& program) = delete;
		GLShader& operator=(const GLShader& program) = delete;
	};
}

#endif // SHADER_PROGRAM_H
