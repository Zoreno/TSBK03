#include "Texture.h"
#include "TextureFile.h"
#include "TGA.h"
#include "BMP.h"
#include <iostream>
#include "Color.h"

Texture2D::Texture2D()
	:_width{ 0 }, _height{ 0 }, _handle{ 0 }, _fileName{ "" }
{}

Texture2D::Texture2D(
	const std::string &filePath,
	TEXTURE_2D_WRAP sWrap,
	TEXTURE_2D_WRAP tWrap,
	TEXTURE_2D_FILTERING magFilter,
	TEXTURE_2D_FILTERING minFilter)
	: _fileName{ filePath }
{
	TextureFile *file{ nullptr };

	if (filePath.find(".tga") != std::string::npos)
	{
		file = new TGA{ filePath.c_str() };
	}
	else if (filePath.find(".bmp") != std::string::npos)
	{
		file = new BMP{ filePath.c_str() };
	}
	else
	{
		throw std::invalid_argument(std::string("Invalid file format. (") + filePath + ")");
	}

	if (file == nullptr)
	{
		throw std::invalid_argument(std::string("Invalid file. (") + filePath + ")");
	}

	glGenTextures(1, &_handle);
	glBindTexture(GL_TEXTURE_2D, _handle);

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		file->hasAlpha() ? GL_RGBA : GL_RGB,
		file->getWidth(),
		file->getHeight(),
		0,
		file->hasAlpha() ? GL_RGBA : GL_RGB,
		GL_UNSIGNED_BYTE,
		file->getPixels().data()
	);

	// Setup S coordinate wrap
	switch (sWrap)
	{
	case TEXTURE_2D_WRAP::REPEAT:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		break;
	case TEXTURE_2D_WRAP::MIRRORED_REPEAT:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		break;
	case TEXTURE_2D_WRAP::CLAMP_TO_EDGE:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		break;
	case TEXTURE_2D_WRAP::CLAMP_TO_BORDER:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		break;
	default:
		std::cerr << "Invalid GL_TEXTURE_WRAP_S parameter to texture " << filePath << std::endl;
		break;
	}

	// Setup T coordinate wrap
	switch (tWrap)
	{
	case TEXTURE_2D_WRAP::REPEAT:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		break;
	case TEXTURE_2D_WRAP::MIRRORED_REPEAT:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		break;
	case TEXTURE_2D_WRAP::CLAMP_TO_EDGE:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		break;
	case TEXTURE_2D_WRAP::CLAMP_TO_BORDER:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	default:
		std::cerr << "Invalid GL_TEXTURE_WRAP_T parameter to texture " << filePath << std::endl;
		break;
	}


	switch (magFilter)
	{
	case TEXTURE_2D_FILTERING::NEAREST:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		break;
	case TEXTURE_2D_FILTERING::LINEAR:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		break;
	default:
		std::cerr << "Invalid GL_TEXTURE_MAG_FILTER parameter to texture " << filePath << std::endl;
		break;
	}

	switch (minFilter)
	{
	case TEXTURE_2D_FILTERING::NEAREST:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		break;
	case TEXTURE_2D_FILTERING::LINEAR:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		break;
	case TEXTURE_2D_FILTERING::NEAREST_MIPMAP_NEAREST:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		break;
	case TEXTURE_2D_FILTERING::LINEAR_MIPMAP_NEAREST:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		break;
	case TEXTURE_2D_FILTERING::NEAREST_MIPMAP_LINEAR:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		break;
	case TEXTURE_2D_FILTERING::LINEAR_MIPMAP_LINEAR:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		break;
	default:
		std::cerr << "Invalid GL_TEXTURE_MIN_FILTER parameter to texture " << filePath << std::endl;
		break;
	}

	glGenerateMipmap(GL_TEXTURE_2D);

	_width = file->getWidth();
	_height = file->getHeight();

	delete file;
}

Texture2D::Texture2D(
	GLuint width,
	GLuint height,
	TEXTURE_2D_FORMAT format,
	TEXTURE_2D_DATATYPE type,
	GLvoid *data)
{
	glGenTextures(1, &_handle);
	glBindTexture(GL_TEXTURE_2D, _handle);
	GLenum f;
	GLenum intf;
	GLenum t;

	switch (format)
	{
	case TEXTURE_2D_FORMAT::DEPTH_COMPONENT:
		f = GL_DEPTH_COMPONENT;
		intf = GL_DEPTH_COMPONENT;
		break;
	case TEXTURE_2D_FORMAT::STENCIL_INDEX:
		f = GL_STENCIL_INDEX;
		intf = GL_STENCIL_INDEX;
		break;
	case TEXTURE_2D_FORMAT::DEPTH_STENCIL:
		f = GL_DEPTH_STENCIL;
		intf = GL_DEPTH_STENCIL;
		break;
	case TEXTURE_2D_FORMAT::RED:
		f = GL_RED;
		intf = GL_RED;
		break;
	case TEXTURE_2D_FORMAT::GREEN:
		f = GL_GREEN;
		intf = GL_GREEN;
		break;
	case TEXTURE_2D_FORMAT::BLUE:
		f = GL_BLUE;
		intf = GL_BLUE;
		break;
	case TEXTURE_2D_FORMAT::RG:
		f = GL_RG;
		intf = GL_RG;
		break;
	case TEXTURE_2D_FORMAT::RGB:
		f = GL_RGB;
		intf = GL_RGB;
		break;
	case TEXTURE_2D_FORMAT::BGR:
		f = GL_BGR;
		intf = GL_BGR;
		break;
	case TEXTURE_2D_FORMAT::RGBA:
		f = GL_RGBA;
		intf = GL_RGBA;
		break;
	case TEXTURE_2D_FORMAT::BGRA:
		f = GL_BGRA;
		intf = GL_BGRA;
		break;
	case TEXTURE_2D_FORMAT::RGB16F:
		f = GL_RGB;
		intf = GL_RGB16F;
		break;
	case TEXTURE_2D_FORMAT::RGBA16F:
		f = GL_RGBA;
		intf = GL_RGBA16F;
		break;
	case TEXTURE_2D_FORMAT::RED_INTEGER:
		f = GL_RED_INTEGER;
		intf = GL_RED_INTEGER;
		break;
	case TEXTURE_2D_FORMAT::GREEN_INTEGER:
		f = GL_GREEN_INTEGER;
		intf = GL_GREEN_INTEGER;
		break;
	case TEXTURE_2D_FORMAT::BLUE_INTEGER:
		f = GL_BLUE_INTEGER;
		intf = GL_BLUE_INTEGER;
		break;
	case TEXTURE_2D_FORMAT::RG_INTEGER:
		f = GL_RG_INTEGER;
		intf = GL_RG_INTEGER;
		break;
	case TEXTURE_2D_FORMAT::RGB_INTEGER:
		f = GL_RGB_INTEGER;
		intf = GL_RGB_INTEGER;
		break;
	case TEXTURE_2D_FORMAT::BGR_INTEGER:
		f = GL_BGR_INTEGER;
		intf = GL_BGR_INTEGER;
		break;
	case TEXTURE_2D_FORMAT::RGBA_INTEGER:
		f = GL_RGBA_INTEGER;
		intf = GL_RGBA_INTEGER;
		break;
	case TEXTURE_2D_FORMAT::BGRA_INTEGER:
		f = GL_BGRA_INTEGER;
		intf = GL_BGRA_INTEGER;
		break;
	default:
		throw std::invalid_argument(std::string("Invalid format."));
	}

	switch (type)
	{
	case TEXTURE_2D_DATATYPE::UNSIGNED_BYTE:
		t = GL_UNSIGNED_BYTE;
		break;
	case TEXTURE_2D_DATATYPE::BYTE:
		t = GL_BYTE;
		break;
	case TEXTURE_2D_DATATYPE::UNSIGNED_SHORT:
		t = GL_UNSIGNED_SHORT;
		break;
	case TEXTURE_2D_DATATYPE::SHORT:
		t = GL_SHORT;
		break;
	case TEXTURE_2D_DATATYPE::UNSIGNED_INT:
		t = GL_UNSIGNED_BYTE;
		break;
	case TEXTURE_2D_DATATYPE::INT:
		t = GL_INT;
		break;
	case TEXTURE_2D_DATATYPE::FLOAT:
		t = GL_FLOAT;
		break;
	default:
		throw std::invalid_argument(std::string("Invalid type."));
	}

	glTexImage2D(
		GL_TEXTURE_2D,	// Target
		0,				// Level
		intf,			// Internal format
		width,			// Width
		height,			// Height
		0,				// Border
		f,				// Format
		t,				// Type
		data			// Data
	);

	glGenerateMipmap(GL_TEXTURE_2D);
}

Texture2D::~Texture2D()
{
	if (_handle != 0)
	{
		glDeleteTextures(1, &_handle);
	}
}

Texture2D::Texture2D(
	Texture2D &&other) noexcept
{
	swap(*this, other);
}

Texture2D & Texture2D::operator=(
	Texture2D &&other) noexcept
{
	swap(*this, other);

	return *this;
}

GLuint Texture2D::getWidth() const
{
	return _width;
}

GLuint Texture2D::getHeight() const
{
	return _height;
}

GLuint Texture2D::getHandle() const
{
	return _handle;
}

void Texture2D::setDepthStencilTextureMode(
	TEXTURE_2D_TEXTURE_MODE mode)
{
	GLint currentTexture;

	glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);
	glBindTexture(GL_TEXTURE_2D, _handle);

	switch (mode)
	{
	case TEXTURE_2D_TEXTURE_MODE::DEPTH_COMPONENT:
		glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_STENCIL_TEXTURE_MODE, GL_DEPTH_COMPONENT);
		break;
	case TEXTURE_2D_TEXTURE_MODE::STENCIL_INDEX:
		// OBS Fel i OpenGL-manualen. Detta är rätt, inte GL_STENCIL_COMPONENT. 
		// Ref: http://stackoverflow.com/questions/31449740/how-to-utilize-gl-arb-stencil-texturing
		glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_STENCIL_TEXTURE_MODE, GL_STENCIL_INDEX);
		break;
	}

	glBindTexture(GL_TEXTURE_2D, currentTexture);
}

void Texture2D::setTextureBorderColor(
	Color color)
{
	GLint currentTexture;

	glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);
	glBindTexture(GL_TEXTURE_2D, _handle);

	float c[4];

	color.getRGBA(c);

	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, c);

	glBindTexture(GL_TEXTURE_2D, currentTexture);
}

void Texture2D::setTextureBaseLevel(
	GLint baseLevel)
{
	GLint currentTexture;

	glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);
	glBindTexture(GL_TEXTURE_2D, _handle);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, baseLevel);

	glBindTexture(GL_TEXTURE_2D, currentTexture);
}

void Texture2D::setTextureMaxLevel(
	GLint maxLevel)
{
	GLint currentTexture;

	glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);
	glBindTexture(GL_TEXTURE_2D, _handle);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, maxLevel);

	glBindTexture(GL_TEXTURE_2D, currentTexture);
}

void Texture2D::setTextureCompareFunc(
	TEXTURE_2D_COMPARE_FUNC func)
{
	GLint currentTexture;

	glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);
	glBindTexture(GL_TEXTURE_2D, _handle);

	switch (func)
	{
	case TEXTURE_2D_COMPARE_FUNC::LESS_EQUAL:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		break;
	case TEXTURE_2D_COMPARE_FUNC::GREATER_EQUAL:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_GEQUAL);
		break;
	case TEXTURE_2D_COMPARE_FUNC::LESS:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
		break;
	case TEXTURE_2D_COMPARE_FUNC::GREATER:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_GREATER);
		break;
	case TEXTURE_2D_COMPARE_FUNC::EQUAL:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_EQUAL);
		break;
	case TEXTURE_2D_COMPARE_FUNC::NOT_EQUAL:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_NOTEQUAL);
		break;
	case TEXTURE_2D_COMPARE_FUNC::ALWAYS:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_ALWAYS);
		break;
	case TEXTURE_2D_COMPARE_FUNC::NEVER:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_NEVER);
		break;
	}

	glBindTexture(GL_TEXTURE_2D, currentTexture);
}

void Texture2D::setTextureCompareMode(
	TEXTURE_2D_COMPARE_MODE mode)
{
	GLint currentTexture;

	glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);
	glBindTexture(GL_TEXTURE_2D, _handle);

	switch (mode)
	{
	case TEXTURE_2D_COMPARE_MODE::COMPARE_REF_TO_TEXTURE:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		break;
	case TEXTURE_2D_COMPARE_MODE::NONE:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
		break;
	}

	glBindTexture(GL_TEXTURE_2D, currentTexture);
}

void Texture2D::setTextureLodBias(
	GLint lodBias)
{
	GLint currentTexture;

	glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);
	glBindTexture(GL_TEXTURE_2D, _handle);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, lodBias);

	glBindTexture(GL_TEXTURE_2D, currentTexture);
}

void Texture2D::setTextureMinLod(
	GLint minLod)
{
	GLint currentTexture;

	glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);
	glBindTexture(GL_TEXTURE_2D, _handle);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, minLod);

	glBindTexture(GL_TEXTURE_2D, currentTexture);
}

void Texture2D::setTextureMaxLod(
	GLint maxLod)
{
	GLint currentTexture;

	glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);
	glBindTexture(GL_TEXTURE_2D, _handle);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, maxLod);

	glBindTexture(GL_TEXTURE_2D, currentTexture);
}

void Texture2D::setTextureWrapS(
	TEXTURE_2D_WRAP sWrap)
{
	GLint currentTexture;

	glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);

	glBindTexture(GL_TEXTURE_2D, _handle);

	switch (sWrap)
	{
	case TEXTURE_2D_WRAP::REPEAT:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		break;
	case TEXTURE_2D_WRAP::MIRRORED_REPEAT:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		break;
	case TEXTURE_2D_WRAP::CLAMP_TO_EDGE:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		break;
	case TEXTURE_2D_WRAP::CLAMP_TO_BORDER:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		break;
	}

	glBindTexture(GL_TEXTURE_2D, currentTexture);
}

void Texture2D::setTextureWrapT(
	TEXTURE_2D_WRAP tWrap)
{
	GLint currentTexture;

	glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);

	glBindTexture(GL_TEXTURE_2D, _handle);

	switch (tWrap)
	{
	case TEXTURE_2D_WRAP::REPEAT:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		break;
	case TEXTURE_2D_WRAP::MIRRORED_REPEAT:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		break;
	case TEXTURE_2D_WRAP::CLAMP_TO_EDGE:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		break;
	case TEXTURE_2D_WRAP::CLAMP_TO_BORDER:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	}

	glBindTexture(GL_TEXTURE_2D, currentTexture);
}

void Texture2D::setTextureWrapR(
	TEXTURE_2D_WRAP rWrap)
{
	GLint currentTexture;

	glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);

	glBindTexture(GL_TEXTURE_2D, _handle);

	switch (rWrap)
	{
	case TEXTURE_2D_WRAP::REPEAT:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		break;
	case TEXTURE_2D_WRAP::MIRRORED_REPEAT:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		break;
	case TEXTURE_2D_WRAP::CLAMP_TO_EDGE:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		break;
	case TEXTURE_2D_WRAP::CLAMP_TO_BORDER:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	}

	glBindTexture(GL_TEXTURE_2D, currentTexture);
}

void Texture2D::setTextureMagFilter(
	TEXTURE_2D_FILTERING magFilter)
{
	GLint currentTexture;

	glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);

	glBindTexture(GL_TEXTURE_2D, _handle);

	switch (magFilter)
	{
	case TEXTURE_2D_FILTERING::NEAREST:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		break;
	case TEXTURE_2D_FILTERING::LINEAR:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		break;
	}

	glBindTexture(GL_TEXTURE_2D, currentTexture);
}

void Texture2D::setTextureMinFilter(
	TEXTURE_2D_FILTERING minFilter)
{
	GLint currentTexture;

	glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);

	glBindTexture(GL_TEXTURE_2D, _handle);

	switch (minFilter)
	{
	case TEXTURE_2D_FILTERING::NEAREST:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		break;
	case TEXTURE_2D_FILTERING::LINEAR:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		break;
	case TEXTURE_2D_FILTERING::NEAREST_MIPMAP_NEAREST:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		break;
	case TEXTURE_2D_FILTERING::LINEAR_MIPMAP_NEAREST:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		break;
	case TEXTURE_2D_FILTERING::NEAREST_MIPMAP_LINEAR:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		break;
	case TEXTURE_2D_FILTERING::LINEAR_MIPMAP_LINEAR:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		break;
	}

	glBindTexture(GL_TEXTURE_2D, currentTexture);
}

void Texture2D::setTextureSwizzleR(
	TEXTURE_2D_SWIZZLE_COMPONENT comp)
{
	GLint currentTexture;

	glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);

	glBindTexture(GL_TEXTURE_2D, _handle);

	switch (comp)
	{
	case TEXTURE_2D_SWIZZLE_COMPONENT::RED:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED);
		break;
	case TEXTURE_2D_SWIZZLE_COMPONENT::GREEN:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_GREEN);
		break;
	case TEXTURE_2D_SWIZZLE_COMPONENT::BLUE:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_BLUE);
		break;
	case TEXTURE_2D_SWIZZLE_COMPONENT::ALPHA:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_ALPHA);
		break;
	case TEXTURE_2D_SWIZZLE_COMPONENT::ZERO:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_ZERO);
		break;
	case TEXTURE_2D_SWIZZLE_COMPONENT::ONE:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_ONE);
		break;
	}

	glBindTexture(GL_TEXTURE_2D, currentTexture);
}

void Texture2D::setTextureSwizzleG(
	TEXTURE_2D_SWIZZLE_COMPONENT comp)
{
	GLint currentTexture;

	glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);

	glBindTexture(GL_TEXTURE_2D, _handle);

	switch (comp)
	{
	case TEXTURE_2D_SWIZZLE_COMPONENT::RED:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED);
		break;
	case TEXTURE_2D_SWIZZLE_COMPONENT::GREEN:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_GREEN);
		break;
	case TEXTURE_2D_SWIZZLE_COMPONENT::BLUE:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_BLUE);
		break;
	case TEXTURE_2D_SWIZZLE_COMPONENT::ALPHA:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_ALPHA);
		break;
	case TEXTURE_2D_SWIZZLE_COMPONENT::ZERO:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_ZERO);
		break;
	case TEXTURE_2D_SWIZZLE_COMPONENT::ONE:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_ONE);
		break;
	}

	glBindTexture(GL_TEXTURE_2D, currentTexture);
}

void Texture2D::setTextureSwizzleB(
	TEXTURE_2D_SWIZZLE_COMPONENT comp)
{
	GLint currentTexture;

	glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);

	glBindTexture(GL_TEXTURE_2D, _handle);

	switch (comp)
	{
	case TEXTURE_2D_SWIZZLE_COMPONENT::RED:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
		break;
	case TEXTURE_2D_SWIZZLE_COMPONENT::GREEN:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_GREEN);
		break;
	case TEXTURE_2D_SWIZZLE_COMPONENT::BLUE:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_BLUE);
		break;
	case TEXTURE_2D_SWIZZLE_COMPONENT::ALPHA:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_ALPHA);
		break;
	case TEXTURE_2D_SWIZZLE_COMPONENT::ZERO:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_ZERO);
		break;
	case TEXTURE_2D_SWIZZLE_COMPONENT::ONE:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_ONE);
		break;
	}

	glBindTexture(GL_TEXTURE_2D, currentTexture);
}

void Texture2D::setTextureSwizzleA(
	TEXTURE_2D_SWIZZLE_COMPONENT comp)
{
	GLint currentTexture;

	glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);

	glBindTexture(GL_TEXTURE_2D, _handle);

	switch (comp)
	{
	case TEXTURE_2D_SWIZZLE_COMPONENT::RED:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_RED);
		break;
	case TEXTURE_2D_SWIZZLE_COMPONENT::GREEN:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_GREEN);
		break;
	case TEXTURE_2D_SWIZZLE_COMPONENT::BLUE:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_BLUE);
		break;
	case TEXTURE_2D_SWIZZLE_COMPONENT::ALPHA:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ALPHA);
		break;
	case TEXTURE_2D_SWIZZLE_COMPONENT::ZERO:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ZERO);
		break;
	case TEXTURE_2D_SWIZZLE_COMPONENT::ONE:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ONE);
		break;
	}

	glBindTexture(GL_TEXTURE_2D, currentTexture);
}

TEXTURE_2D_TEXTURE_MODE Texture2D::getDepthStencilTextureMode() const
{
	GLint mode;
	TEXTURE_2D_TEXTURE_MODE returnVal;
	GLint currentTexture;

	// Save the currently bound texture
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);
	glBindTexture(GL_TEXTURE_2D, _handle);

	glGetTexParameteriv(GL_TEXTURE_2D, GL_DEPTH_STENCIL_TEXTURE_MODE, &mode);

	switch (static_cast<GLuint>(mode))
	{
	case GL_DEPTH_COMPONENT:
		returnVal = TEXTURE_2D_TEXTURE_MODE::DEPTH_COMPONENT;
		break;
	case GL_STENCIL_INDEX:
		returnVal = TEXTURE_2D_TEXTURE_MODE::STENCIL_INDEX;
		break;
	default:
		returnVal = TEXTURE_2D_TEXTURE_MODE::INVALID_VALUE;
	}

	glBindTexture(GL_TEXTURE_2D, currentTexture);

	return returnVal;
}

Color Texture2D::getTextureBorderColor() const
{
	float color[4];
	Color returnVal;
	GLint currentTexture;

	glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);
	glBindTexture(GL_TEXTURE_2D, _handle);

	glGetTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);

	returnVal = Color(color);

	glBindTexture(GL_TEXTURE_2D, currentTexture);

	return returnVal;
}

GLint Texture2D::getTextureBaseLevel() const
{
	GLint returnVal;
	GLint currentTexture;

	glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);
	glBindTexture(GL_TEXTURE_2D, _handle);

	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, &returnVal);

	glBindTexture(GL_TEXTURE_2D, currentTexture);

	return returnVal;
}

GLint Texture2D::getTextureMaxLevel() const
{
	GLint returnVal;
	GLint currentTexture;

	glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);
	glBindTexture(GL_TEXTURE_2D, _handle);

	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, &returnVal);

	glBindTexture(GL_TEXTURE_2D, currentTexture);

	return returnVal;
}

TEXTURE_2D_COMPARE_FUNC Texture2D::getTextureCompareFunc() const
{
	GLint compareFunc;
	TEXTURE_2D_COMPARE_FUNC returnVal;
	GLint currentTexture;

	glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);
	glBindTexture(GL_TEXTURE_2D, _handle);

	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, &compareFunc);

	switch (compareFunc)
	{
	case GL_LEQUAL:
		returnVal = TEXTURE_2D_COMPARE_FUNC::LESS_EQUAL;
		break;
	case GL_GEQUAL:
		returnVal = TEXTURE_2D_COMPARE_FUNC::GREATER_EQUAL;
		break;
	case GL_LESS:
		returnVal = TEXTURE_2D_COMPARE_FUNC::LESS;
		break;
	case GL_GREATER:
		returnVal = TEXTURE_2D_COMPARE_FUNC::GREATER;
		break;
	case GL_EQUAL:
		returnVal = TEXTURE_2D_COMPARE_FUNC::EQUAL;
		break;
	case GL_NOTEQUAL:
		returnVal = TEXTURE_2D_COMPARE_FUNC::NOT_EQUAL;
		break;
	case GL_ALWAYS:
		returnVal = TEXTURE_2D_COMPARE_FUNC::ALWAYS;
		break;
	case GL_NEVER:
		returnVal = TEXTURE_2D_COMPARE_FUNC::NEVER;
		break;
	default:
		returnVal = TEXTURE_2D_COMPARE_FUNC::INVALID_VALUE;
	}

	glBindTexture(GL_TEXTURE_2D, currentTexture);

	return returnVal;
}

TEXTURE_2D_COMPARE_MODE Texture2D::getTextureCompareMode() const
{
	GLint mode;
	TEXTURE_2D_COMPARE_MODE returnVal;
	GLint currentTexture;

	glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);
	glBindTexture(GL_TEXTURE_2D, _handle);

	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, &mode);

	switch (mode)
	{
	case GL_COMPARE_REF_TO_TEXTURE:
		returnVal = TEXTURE_2D_COMPARE_MODE::COMPARE_REF_TO_TEXTURE;
		break;
	case GL_NONE:
		returnVal = TEXTURE_2D_COMPARE_MODE::NONE;
		break;
	default:
		returnVal = TEXTURE_2D_COMPARE_MODE::INVALID_VALUE;
	}

	glBindTexture(GL_TEXTURE_2D, currentTexture);

	return returnVal;
}

GLint Texture2D::getTextureLodBias() const
{
	GLint returnVal;
	GLint currentTexture;

	glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);
	glBindTexture(GL_TEXTURE_2D, _handle);

	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, &returnVal);

	glBindTexture(GL_TEXTURE_2D, currentTexture);

	return returnVal;
}

GLint Texture2D::getTextureMinLod() const
{
	GLint returnVal;
	GLint currentTexture;

	glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);
	glBindTexture(GL_TEXTURE_2D, _handle);

	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, &returnVal);

	glBindTexture(GL_TEXTURE_2D, currentTexture);

	return returnVal;
}

GLint Texture2D::getTextureMaxLod() const
{

	GLint returnVal;
	GLint currentTexture;

	glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);
	glBindTexture(GL_TEXTURE_2D, _handle);

	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, &returnVal);

	glBindTexture(GL_TEXTURE_2D, currentTexture);

	return returnVal;
}

TEXTURE_2D_WRAP Texture2D::getTextureWrapS() const
{
	GLint sWrap;
	TEXTURE_2D_WRAP returnVal;
	GLint currentTexture;

	// Save the currently bound texture
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);
	glBindTexture(GL_TEXTURE_2D, _handle);

	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &sWrap);

	switch (static_cast<GLuint>(sWrap))
	{
	case GL_REPEAT:
		returnVal = TEXTURE_2D_WRAP::REPEAT;
		break;
	case GL_MIRRORED_REPEAT:
		returnVal = TEXTURE_2D_WRAP::MIRRORED_REPEAT;
		break;
	case GL_CLAMP_TO_EDGE:
		returnVal = TEXTURE_2D_WRAP::CLAMP_TO_EDGE;
		break;
	case GL_CLAMP_TO_BORDER:
		returnVal = TEXTURE_2D_WRAP::CLAMP_TO_BORDER;
		break;
	default:
		returnVal = TEXTURE_2D_WRAP::INVALID_VALUE;
	}

	glBindTexture(GL_TEXTURE_2D, currentTexture);

	return returnVal;
}

TEXTURE_2D_WRAP Texture2D::getTextureWrapT() const
{
	GLint tWrap;
	TEXTURE_2D_WRAP returnVal;
	GLint currentTexture;

	// Save the currently bound texture
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);
	glBindTexture(GL_TEXTURE_2D, _handle);

	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &tWrap);

	switch (static_cast<GLuint>(tWrap))
	{
	case GL_REPEAT:
		returnVal = TEXTURE_2D_WRAP::REPEAT;
		break;
	case GL_MIRRORED_REPEAT:
		returnVal = TEXTURE_2D_WRAP::MIRRORED_REPEAT;
		break;
	case GL_CLAMP_TO_EDGE:
		returnVal = TEXTURE_2D_WRAP::CLAMP_TO_EDGE;
		break;
	case GL_CLAMP_TO_BORDER:
		returnVal = TEXTURE_2D_WRAP::CLAMP_TO_BORDER;
		break;
	default:
		return TEXTURE_2D_WRAP::INVALID_VALUE;
	}

	glBindTexture(GL_TEXTURE_2D, currentTexture);

	return returnVal;
}

TEXTURE_2D_WRAP Texture2D::getTextureWrapR() const
{
	GLint rWrap;
	TEXTURE_2D_WRAP returnVal;
	GLint currentTexture;

	// Save the currently bound texture
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);
	glBindTexture(GL_TEXTURE_2D, _handle);

	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, &rWrap);

	switch (static_cast<GLuint>(rWrap))
	{
	case GL_REPEAT:
		returnVal = TEXTURE_2D_WRAP::REPEAT;
		break;
	case GL_MIRRORED_REPEAT:
		returnVal = TEXTURE_2D_WRAP::MIRRORED_REPEAT;
		break;
	case GL_CLAMP_TO_EDGE:
		returnVal = TEXTURE_2D_WRAP::CLAMP_TO_EDGE;
		break;
	case GL_CLAMP_TO_BORDER:
		returnVal = TEXTURE_2D_WRAP::CLAMP_TO_BORDER;
		break;
	default:
		return TEXTURE_2D_WRAP::INVALID_VALUE;
	}

	glBindTexture(GL_TEXTURE_2D, currentTexture);

	return returnVal;
}

TEXTURE_2D_FILTERING Texture2D::getTextureMagFilter() const
{
	GLint magFilter;
	TEXTURE_2D_FILTERING returnVal;
	GLint currentTexture;

	// Save the currently bound texture
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);
	glBindTexture(GL_TEXTURE_2D, _handle);

	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &magFilter);

	switch (static_cast<GLuint>(magFilter))
	{
	case GL_NEAREST:
		returnVal = TEXTURE_2D_FILTERING::NEAREST;
		break;
	case GL_LINEAR:
		returnVal = TEXTURE_2D_FILTERING::LINEAR;
		break;
	default:
		returnVal = TEXTURE_2D_FILTERING::INVALID_VALUE;
	}

	glBindTexture(GL_TEXTURE_2D, currentTexture);

	return returnVal;
}

TEXTURE_2D_FILTERING Texture2D::getTextureMinFilter() const
{
	GLint minFilter;
	TEXTURE_2D_FILTERING returnVal;
	GLint currentTexture;

	// Save the currently bound texture
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);
	glBindTexture(GL_TEXTURE_2D, _handle);

	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &minFilter);

	switch (static_cast<GLuint>(minFilter))
	{
	case GL_NEAREST:
		returnVal = TEXTURE_2D_FILTERING::NEAREST;
		break;
	case GL_LINEAR:
		returnVal = TEXTURE_2D_FILTERING::LINEAR;
		break;
	case GL_NEAREST_MIPMAP_NEAREST:
		returnVal = TEXTURE_2D_FILTERING::NEAREST_MIPMAP_NEAREST;
		break;
	case GL_LINEAR_MIPMAP_NEAREST:
		returnVal = TEXTURE_2D_FILTERING::LINEAR_MIPMAP_NEAREST;
		break;
	case GL_NEAREST_MIPMAP_LINEAR:
		returnVal = TEXTURE_2D_FILTERING::NEAREST_MIPMAP_LINEAR;
		break;
	case GL_LINEAR_MIPMAP_LINEAR:
		returnVal = TEXTURE_2D_FILTERING::LINEAR_MIPMAP_LINEAR;
		break;
	default:
		returnVal = TEXTURE_2D_FILTERING::INVALID_VALUE;
	}

	glBindTexture(GL_TEXTURE_2D, currentTexture);

	return returnVal;
}

TEXTURE_2D_SWIZZLE_COMPONENT Texture2D::getTextureSwizzleR() const
{

	GLint comp;
	TEXTURE_2D_SWIZZLE_COMPONENT returnVal;
	GLint currentTexture;

	// Save the currently bound texture
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);
	glBindTexture(GL_TEXTURE_2D, _handle);

	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, &comp);

	switch (comp)
	{
	case GL_RED:
		returnVal = TEXTURE_2D_SWIZZLE_COMPONENT::RED;
		break;
	case GL_GREEN:
		returnVal = TEXTURE_2D_SWIZZLE_COMPONENT::GREEN;
		break;
	case GL_BLUE:
		returnVal = TEXTURE_2D_SWIZZLE_COMPONENT::BLUE;
		break;
	case GL_ALPHA:
		returnVal = TEXTURE_2D_SWIZZLE_COMPONENT::ALPHA;
		break;
	case GL_ZERO:
		returnVal = TEXTURE_2D_SWIZZLE_COMPONENT::ZERO;
		break;
	case GL_ONE:
		returnVal = TEXTURE_2D_SWIZZLE_COMPONENT::ONE;
		break;
	default:
		returnVal = TEXTURE_2D_SWIZZLE_COMPONENT::INVALID_VALUE;
	}

	glBindTexture(GL_TEXTURE_2D, currentTexture);

	return returnVal;
}

TEXTURE_2D_SWIZZLE_COMPONENT Texture2D::getTextureSwizzleG() const
{
	GLint comp;
	TEXTURE_2D_SWIZZLE_COMPONENT returnVal;
	GLint currentTexture;

	// Save the currently bound texture
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);
	glBindTexture(GL_TEXTURE_2D, _handle);

	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, &comp);

	switch (comp)
	{
	case GL_RED:
		returnVal = TEXTURE_2D_SWIZZLE_COMPONENT::RED;
		break;
	case GL_GREEN:
		returnVal = TEXTURE_2D_SWIZZLE_COMPONENT::GREEN;
		break;
	case GL_BLUE:
		returnVal = TEXTURE_2D_SWIZZLE_COMPONENT::BLUE;
		break;
	case GL_ALPHA:
		returnVal = TEXTURE_2D_SWIZZLE_COMPONENT::ALPHA;
		break;
	case GL_ZERO:
		returnVal = TEXTURE_2D_SWIZZLE_COMPONENT::ZERO;
		break;
	case GL_ONE:
		returnVal = TEXTURE_2D_SWIZZLE_COMPONENT::ONE;
		break;
	default:
		returnVal = TEXTURE_2D_SWIZZLE_COMPONENT::INVALID_VALUE;
	}

	glBindTexture(GL_TEXTURE_2D, currentTexture);

	return returnVal;
}

TEXTURE_2D_SWIZZLE_COMPONENT Texture2D::getTextureSwizzleB() const
{
	GLint comp;
	TEXTURE_2D_SWIZZLE_COMPONENT returnVal;
	GLint currentTexture;

	// Save the currently bound texture
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);
	glBindTexture(GL_TEXTURE_2D, _handle);

	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, &comp);

	switch (comp)
	{
	case GL_RED:
		returnVal = TEXTURE_2D_SWIZZLE_COMPONENT::RED;
		break;
	case GL_GREEN:
		returnVal = TEXTURE_2D_SWIZZLE_COMPONENT::GREEN;
		break;
	case GL_BLUE:
		returnVal = TEXTURE_2D_SWIZZLE_COMPONENT::BLUE;
		break;
	case GL_ALPHA:
		returnVal = TEXTURE_2D_SWIZZLE_COMPONENT::ALPHA;
		break;
	case GL_ZERO:
		returnVal = TEXTURE_2D_SWIZZLE_COMPONENT::ZERO;
		break;
	case GL_ONE:
		returnVal = TEXTURE_2D_SWIZZLE_COMPONENT::ONE;
		break;
	default:
		returnVal = TEXTURE_2D_SWIZZLE_COMPONENT::INVALID_VALUE;
	}

	glBindTexture(GL_TEXTURE_2D, currentTexture);

	return returnVal;
}

TEXTURE_2D_SWIZZLE_COMPONENT Texture2D::getTextureSwizzleA() const
{

	GLint comp;
	TEXTURE_2D_SWIZZLE_COMPONENT returnVal;
	GLint currentTexture;

	// Save the currently bound texture
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);
	glBindTexture(GL_TEXTURE_2D, _handle);

	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, &comp);

	switch (comp)
	{
	case GL_RED:
		returnVal = TEXTURE_2D_SWIZZLE_COMPONENT::RED;
		break;
	case GL_GREEN:
		returnVal = TEXTURE_2D_SWIZZLE_COMPONENT::GREEN;
		break;
	case GL_BLUE:
		returnVal = TEXTURE_2D_SWIZZLE_COMPONENT::BLUE;
		break;
	case GL_ALPHA:
		returnVal = TEXTURE_2D_SWIZZLE_COMPONENT::ALPHA;
		break;
	case GL_ZERO:
		returnVal = TEXTURE_2D_SWIZZLE_COMPONENT::ZERO;
		break;
	case GL_ONE:
		returnVal = TEXTURE_2D_SWIZZLE_COMPONENT::ONE;
		break;
	default:
		returnVal = TEXTURE_2D_SWIZZLE_COMPONENT::INVALID_VALUE;
	}

	glBindTexture(GL_TEXTURE_2D, currentTexture);

	return returnVal;
}

void Texture2D::bind() const
{
	glBindTexture(GL_TEXTURE_2D, _handle);
}

void Texture2D::bind(
	GLuint texUnit) const
{
	GLint numTextureUnits;

	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &numTextureUnits);

	if (texUnit >= static_cast<GLuint>(numTextureUnits))
	{
		throw std::invalid_argument("Requested texture unit larger than supported");
	}

	glActiveTexture(GL_TEXTURE0 + texUnit);
	glBindTexture(GL_TEXTURE_2D, _handle);
}

const std::string & Texture2D::getFilename() const
{
	return _fileName;
}

void swap(
	Texture2D &lhs,
	Texture2D &rhs) noexcept
{
	std::swap(lhs._width, rhs._width);
	std::swap(lhs._height, rhs._height);
	std::swap(lhs._handle, rhs._handle);
}

TextureCubeMap::TextureCubeMap(
	const std::string &posXfile,
	const std::string &negXfile,
	const std::string &posYfile,
	const std::string &negYfile,
	const std::string &posZfile,
	const std::string &negZfile)
{
	_fileNames[0] = posXfile;
	_fileNames[1] = negXfile;
	_fileNames[2] = posYfile;
	_fileNames[3] = negYfile;
	_fileNames[4] = posZfile;
	_fileNames[5] = negZfile;

	glGenTextures(1, &_handle);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _handle);

	for (unsigned int i = 0; i < 6; ++i)
	{
		TextureFile *file;

		if (_fileNames[i].find(".tga") != std::string::npos)
		{
			file = new TGA{ _fileNames[i].c_str() };
		}
		else if (_fileNames[i].find(".bmp") != std::string::npos)
		{
			file = new BMP{ _fileNames[i].c_str() };
		}
		else
		{
			throw std::invalid_argument(std::string("Invalid file format. (") + _fileNames[i] + ")");
		}

		if (file == nullptr)
		{
			throw std::invalid_argument(std::string("Invalid file. (") + _fileNames[i] + ")");
		}

		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0,
			file->hasAlpha() ? GL_RGBA : GL_RGB,
			file->getWidth(),
			file->getHeight(),
			0,
			file->hasAlpha() ? GL_RGBA : GL_RGB,
			GL_UNSIGNED_BYTE,
			file->getPixels().data()
		);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);


		delete file;
	}


}

TextureCubeMap::~TextureCubeMap()
{
	glDeleteTextures(1, &_handle);
}

void TextureCubeMap::bind() const
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, _handle);
}

void TextureCubeMap::bind(
	GLuint texUnit) const
{
	GLint numTextureUnits;

	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &numTextureUnits);

	if (texUnit >= static_cast<GLuint>(numTextureUnits))
	{
		throw std::invalid_argument("Requested texture unit larger than supported");
	}

	glActiveTexture(GL_TEXTURE0 + texUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _handle);
}

const std::string & TextureCubeMap::getFilename(
	unsigned int face)
{
	return _fileNames[face];
}

GLuint TextureCubeMap::getHandle() const
{
	return _handle;
}
