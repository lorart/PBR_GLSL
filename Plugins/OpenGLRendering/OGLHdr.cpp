#include "OGLHdr.h"

NCL::Rendering::OGLHdr::OGLHdr(std::string& HdrFilename)
{
	//HdrTexture=(OGLTexture*)TextureLoader::LoadAPITexture(HdrFilename);
	HdrTexture = (OGLTexture*)NCL::Rendering::OGLTexture::LinerRGBATextureFromFilename(HdrFilename);
	
	cubeModel= new Model(Assets::MESHDIR + "CUBE" + ".obj", 0);
	//model->meshes[i]->material->matShader
	//todo:check
	HdrShader = new OGLShader("HDR_Vert.glsl", "HDR_Frag.glsl");
	cubeModel->meshes[0]->material->matShader= HdrShader;
	
	
}

NCL::Rendering::OGLHdr::~OGLHdr()
{
	delete cubeModel;
	delete HdrTexture;
}
