#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

class Color;

enum class TEXTURE_2D_TEXTURE_MODE
{
	STENCIL_INDEX,
	DEPTH_COMPONENT,
	INVALID_VALUE
};

enum class TEXTURE_2D_COMPARE_FUNC
{
	LESS_EQUAL,
	GREATER_EQUAL,
	LESS,
	GREATER,
	EQUAL,
	NOT_EQUAL,
	ALWAYS,
	NEVER,
	INVALID_VALUE
};

enum class TEXTURE_2D_COMPARE_MODE
{
	COMPARE_REF_TO_TEXTURE,
	NONE,
	INVALID_VALUE
};

enum class TEXTURE_2D_SWIZZLE_COMPONENT
{
	RED,
	GREEN,
	BLUE,
	ALPHA,
	ZERO,
	ONE,
	INVALID_VALUE
};

enum class TEXTURE_2D_WRAP
{
	REPEAT,
	MIRRORED_REPEAT,
	CLAMP_TO_EDGE,
	CLAMP_TO_BORDER,
	INVALID_VALUE
};

enum class TEXTURE_2D_FILTERING
{
	NEAREST,
	LINEAR,
	NEAREST_MIPMAP_NEAREST,
	LINEAR_MIPMAP_NEAREST,
	NEAREST_MIPMAP_LINEAR,
	LINEAR_MIPMAP_LINEAR,
	INVALID_VALUE
};

// TODO: Add floating point texture formats
enum class TEXTURE_2D_FORMAT
{
	DEPTH_COMPONENT,
	STENCIL_INDEX,
	DEPTH_STENCIL,

	RED,
	GREEN,
	BLUE,

	RG,
	RGB,
	BGR,
	RGBA,
	BGRA,

	RGB16F,
	RGBA16F,

	RED_INTEGER,
	GREEN_INTEGER,
	BLUE_INTEGER,

	RG_INTEGER,
	RGB_INTEGER,
	BGR_INTEGER,
	RGBA_INTEGER,
	BGRA_INTEGER,

	INVALID_VALUE
};

enum class TEXTURE_2D_DATATYPE
{
	UNSIGNED_BYTE,
	BYTE,

	UNSIGNED_SHORT,
	SHORT,

	UNSIGNED_INT,
	INT,

	FLOAT,

	INVALID_VALUE
};

// TODO: Handle JPEG files

class Texture2D
{
public:
	Texture2D();

	explicit Texture2D(
		const std::string& filePath,
		TEXTURE_2D_WRAP sWrap = TEXTURE_2D_WRAP::REPEAT,
		TEXTURE_2D_WRAP tWrap = TEXTURE_2D_WRAP::REPEAT,
		TEXTURE_2D_FILTERING magFilter = TEXTURE_2D_FILTERING::LINEAR,
		TEXTURE_2D_FILTERING minFilter = TEXTURE_2D_FILTERING::LINEAR_MIPMAP_LINEAR);

	Texture2D(
		GLuint width,
		GLuint height,
		TEXTURE_2D_FORMAT format,
		TEXTURE_2D_DATATYPE type,
		GLvoid *data = nullptr);

	~Texture2D();

	Texture2D(const Texture2D& other) = delete;
	Texture2D(Texture2D&& other) noexcept;

	Texture2D& operator=(const Texture2D& other) = delete;
	Texture2D& operator=(Texture2D&& other) noexcept;

	GLuint getWidth() const;
	GLuint getHeight() const;

	GLuint getHandle() const;

	void setDepthStencilTextureMode(TEXTURE_2D_TEXTURE_MODE mode);
	void setTextureBorderColor(Color color);
	void setTextureBaseLevel(GLint baseLevel);
	void setTextureMaxLevel(GLint maxLevel);
	void setTextureCompareFunc(TEXTURE_2D_COMPARE_FUNC func);
	void setTextureCompareMode(TEXTURE_2D_COMPARE_MODE mode);
	void setTextureLodBias(GLint lodBias);
	void setTextureMinLod(GLint minLod);
	void setTextureMaxLod(GLint maxLod);
	void setTextureWrapS(TEXTURE_2D_WRAP sWrap);
	void setTextureWrapT(TEXTURE_2D_WRAP tWrap);
	void setTextureWrapR(TEXTURE_2D_WRAP rWrap);
	void setTextureMagFilter(TEXTURE_2D_FILTERING magFilter);
	void setTextureMinFilter(TEXTURE_2D_FILTERING minFilter);
	void setTextureSwizzleR(TEXTURE_2D_SWIZZLE_COMPONENT comp);
	void setTextureSwizzleG(TEXTURE_2D_SWIZZLE_COMPONENT comp);
	void setTextureSwizzleB(TEXTURE_2D_SWIZZLE_COMPONENT comp);
	void setTextureSwizzleA(TEXTURE_2D_SWIZZLE_COMPONENT comp);

	TEXTURE_2D_TEXTURE_MODE getDepthStencilTextureMode() const;
	Color getTextureBorderColor() const;
	GLint getTextureBaseLevel() const;
	GLint getTextureMaxLevel() const;
	TEXTURE_2D_COMPARE_FUNC getTextureCompareFunc() const;
	TEXTURE_2D_COMPARE_MODE getTextureCompareMode() const;
	GLint getTextureLodBias() const;
	GLint getTextureMinLod() const;
	GLint getTextureMaxLod() const;
	TEXTURE_2D_WRAP getTextureWrapS() const;
	TEXTURE_2D_WRAP getTextureWrapT() const;
	TEXTURE_2D_WRAP getTextureWrapR() const;
	TEXTURE_2D_FILTERING getTextureMagFilter() const;
	TEXTURE_2D_FILTERING getTextureMinFilter() const;
	TEXTURE_2D_SWIZZLE_COMPONENT getTextureSwizzleR() const;
	TEXTURE_2D_SWIZZLE_COMPONENT getTextureSwizzleG() const;
	TEXTURE_2D_SWIZZLE_COMPONENT getTextureSwizzleB() const;
	TEXTURE_2D_SWIZZLE_COMPONENT getTextureSwizzleA() const;

	void bind() const;
	void bind(GLuint texUnit) const;

	const std::string& getFilename() const;

private:
	GLuint _width;
	GLuint _height;

	GLuint _handle;

	std::string _fileName;

	friend void swap(Texture2D& lhs, Texture2D& rhs) noexcept;
};

// TODO: Fix with texture parameters as Texture2D
class TextureCubeMap
{
public:
	TextureCubeMap(
		const std::string& posXfile,
		const std::string& negXfile,
		const std::string& posYfile,
		const std::string& negYfile,
		const std::string& posZfile,
		const std::string& negZfile);

	~TextureCubeMap();

	void bind() const;
	void bind(GLuint texUnit) const;

	const std::string& getFilename(unsigned int face);

	GLuint getHandle() const;

private:

	std::string _fileNames[6];

	GLuint _handle;
};