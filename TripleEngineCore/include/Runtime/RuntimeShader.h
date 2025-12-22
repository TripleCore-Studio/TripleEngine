#ifndef RUNTIME_SHADER_H
#define RUNTIME_SHADER_H

namespace TripleEngineCore::Runtime
{
	struct RuntimeShader
	{
	public:
		RuntimeShader() : vertexShaderCode(nullptr), fragmentShaderCode(nullptr) {}
		RuntimeShader(const char* vs, const char* fs, uint64_t vertexHash, uint64_t fragmentHash)
			: vertexShaderCode(vs),
			fragmentShaderCode(fs),
			vertexHash(vertexHash),
			fragmentHash(fragmentHash) {}

		const char* vertexShaderCode;
		const char* fragmentShaderCode;

		uint64_t vertexHash = 0;
		uint64_t fragmentHash = 0;
	};
}

#endif // RUNTIME_SHADER_H
