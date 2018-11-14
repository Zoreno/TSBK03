#pragma once

#include <string>

using GLSLTessEvalShaderHandle = unsigned int;

class GLSLTessEvalShader
{
public:
	explicit GLSLTessEvalShader(const std::string& path);

	~GLSLTessEvalShader();

	int compile();

	const std::string& getInfoLog() const;

	GLSLTessEvalShaderHandle getHandle() const;
private:
	std::string _path;

	GLSLTessEvalShaderHandle _handle;

	std::string _infoLog{};
};
