#include "OGLHdr.h"

NCL::Rendering::OGLHdr::OGLHdr(std::string& HdrFilename)
{
	HdrTexture = (OGLTexture*)NCL::Rendering::OGLTexture::LinerRGBATextureFromFilename(HdrFilename);

	cubeTexSize = 512;
	cubeIrradianceTexSize = 32;
	prefilterMapTexSize = 128;


	cubeTex = (OGLTexture*)NCL::Rendering::OGLTexture::AllocateCubeTexture(cubeTexSize);
	irradianceMap= (OGLTexture*)NCL::Rendering::OGLTexture::AllocateCubeTexture(cubeIrradianceTexSize);
    prefilterMap= (OGLTexture*)NCL::Rendering::OGLTexture::AllocateCubeTexture(prefilterMapTexSize);
	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap->GetObjectID());
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);



	cubeModel= new Model(Assets::MESHDIR + "CUBE" + ".obj", 0);

	

	HdrToCubemapShader = new OGLShader("HDR_toCubeMap_Vert.glsl", "HDR_toCubeMap_Frag.glsl");
	SkyboxShader = new OGLShader("Skybox_Vert.glsl","Skybox_Frag.glsl");
	irradianceShader = new OGLShader("HDR_toCubeMap_Vert.glsl","HDR_irradiance_Frag.glsl");
	prefilterShader = new OGLShader("HDR_toCubeMap_Vert.glsl","preFilter_Frag.glsl");
	


	//irradianceShader = new  OGLShader("HDR_irradiance_Vert.glsl","HDR_irradiance_Vert.glsl");
	generate_bind_Fbo(captureFBO);
	generate_bind_Rbo(captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, cubeTexSize, cubeTexSize);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
	clear_Fbo_Rbo();

	generate_bind_Fbo(captureFBO_irr);
	generate_bind_Rbo(captureRBO_irr);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, cubeIrradianceTexSize, cubeIrradianceTexSize);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO_irr);
	clear_Fbo_Rbo();

	//generate_bind_Fbo(captureFBO_pre);
	//generate_bind_Rbo(captureRBO_pre);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, prefilterMapTexSize, prefilterMapTexSize);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO_pre);
	//clear_Fbo_Rbo();



	
	
}

NCL::Rendering::OGLHdr::~OGLHdr()
{
	delete cubeModel;
	delete HdrTexture;
	//delete HdrShader;
	delete HdrToCubemapShader;
	delete SkyboxShader;
	delete irradianceShader;
	delete irradianceMap;
	delete prefilterShader;
	delete prefilterMap;

}

