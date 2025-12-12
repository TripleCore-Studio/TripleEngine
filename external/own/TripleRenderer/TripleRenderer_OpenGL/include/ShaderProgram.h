#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include <string>

namespace TripleEngineCore {
	namespace TripleRenderer {
		class ShaderProgram {
		public:
			ShaderProgram();
			bool compileProgram(const std::string& verSource, const std::string& fragSource);
			void use();
			void setUniformMat4(const std::string& name, const float* matrix);
			void setUniform1f(const std::string& name, float f);
			bool isCompiled() const { return _compiled; }
			std::string getErrorLog() const { return _errorLog; }
			unsigned int getProgramID() const { return _programID; }
			bool deleteProgram();
			~ShaderProgram();
		private:
			unsigned int _programID;
			bool _compiled;
			std::string _errorLog;
			unsigned int compileShader(const std::string& source, unsigned int shaderType, std::string& outLog);
			unsigned int linkProgram(unsigned int vertexShader, unsigned int fragmentShader, std::string& outLog);

			bool operator==(const ShaderProgram& other) const;
			bool operator!=(const ShaderProgram& other) const;

			ShaderProgram(ShaderProgram&& other) noexcept;
			ShaderProgram& operator=(ShaderProgram&& other) noexcept;

			ShaderProgram(const ShaderProgram& program) = delete;
			ShaderProgram& operator=(const ShaderProgram& program) = delete;
		};
	}
}

#endif // SHADER_PROGRAM_H
