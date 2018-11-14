#pragma once

#include <string>

using GLSLVertexShaderHandle = unsigned int;

class GLSLVertexShader
{
public:
	explicit GLSLVertexShader(const std::string& path);

	~GLSLVertexShader();

	int compile();

	const std::string& getInfoLog() const;

	GLSLVertexShaderHandle getHandle() const;
private:
	std::string _path;

	GLSLVertexShaderHandle _handle;

	std::string _infoLog{};
};
