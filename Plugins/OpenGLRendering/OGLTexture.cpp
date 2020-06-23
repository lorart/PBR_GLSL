/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#include "OGLTexture.h"
#include "OGLRenderer.h"

#include "../../Common/TextureLoader.h"

using namespace NCL;
using namespace NCL::Rendering;

OGLTexture::OGLTexture()
{
	glGenTextures(1, &texID);
}

OGLTexture::OGLTexture(GLuint texToOwn) {
	texID = texToOwn;
}

OGLTexture::~OGLTexture()
{

	glDeleteTextures(1, &texID);
}

TextureBase* OGLTexture::RGBATextureFromData(char* data, int width, int height, int channels) {
	OGLTexture* tex = new OGLTexture();

	int dataSize = width * height * channels; //This always assumes data is 1 byte per channel

	int sourceType = GL_RGB;

	switch (channels) {
		case 1: sourceType = GL_RED	; break;

		case 2: sourceType = GL_RG	; break;
		case 3: sourceType = GL_RGB	; break;
		case 4: sourceType = GL_RGBA; break;
		//default:
	}

	glBindTexture(GL_TEXTURE_2D, tex->texID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, sourceType, GL_UNSIGNED_BYTE, data);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	return tex;
}

TextureBase* NCL::Rendering::OGLTexture::AllocateCubeTexture()
{
	OGLTexture* tex = new OGLTexture();
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex->texID);


	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0); //todo:check
	return tex;
}

TextureBase* OGLTexture::RGBATextureFromFilename(const std::string&name) {
	char* texData	= nullptr;
	int width		= 0;
	int height		= 0;
	int channels	= 0;
	int flags		= 0;
	TextureLoader::LoadTexture(name, texData, width, height, channels, flags);

	TextureBase* glTex = RGBATextureFromData(texData, width, height, channels);

	free(texData);

	return glTex;
}

TextureBase* OGLTexture::LinerRGBATextureFromData(char* data, int width, int height, int channels) {
	OGLTexture* tex = new OGLTexture();

	int dataSize = width * height * channels; //This always assumes data is 1 byte per channel
	/*
	int sourceType = GL_RGB;

	switch (channels) {
	case 1: sourceType = GL_RED; break;

	case 2: sourceType = GL_RG; break;
	case 3: sourceType = GL_RGB; break;
	case 4: sourceType = GL_RGBA; break;
		//default:
	}
	*/

	glBindTexture(GL_TEXTURE_2D, tex->texID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGB, GL_FLOAT, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	return tex;
}
//todo::
NCL::Rendering::TextureBase* NCL::Rendering::OGLTexture::LinerRGBATextureFromFilename(const std::string& name)
{
	char* texData = nullptr;
	int width = 0;
	int height = 0;
	int channels = 0;
	int flags = 0;
	TextureLoader::LoadLinearTexture(name, texData, width, height, channels, flags);//todo:check

	//todo::delete
	std::cout << " name" << name<< " texData" <<texData<< " width" << width<< " height" << height<< "  channels" << channels<< "  flags" << flags<<std::endl;

	TextureBase* glTex = LinerRGBATextureFromData(texData, width, height, channels);

	free(texData);

	return glTex;

}


