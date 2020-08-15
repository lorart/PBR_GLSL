#include "OGLHdr.h"

NCL::Rendering::OGLHdr::OGLHdr(std::string& HdrFilename)
{
	HdrTexture = (OGLTexture*)NCL::Rendering::OGLTexture::LinerRGBATextureFromFilename(HdrFilename);

	cubeTexSize = 512;
	cubeIrradianceTexSize = 32;
	prefilterMapTexSize = 128;
	brdfLutTexSize = 512;

	brdfLutTex_Load = (OGLTexture*)TextureLoader::LoadAPITexture("ibl_brdf_lut.jpg");

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
	brdfLutShader= new OGLShader("BrdfLut_Vert.glsl", "BrdfLut_Frag1.glsl");
	


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



	glGenFramebuffers(1, &captureFBO_pre);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO_pre);
	glGenRenderbuffers(1, &captureRBO_pre);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO_pre);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, prefilterMapTexSize, prefilterMapTexSize);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO_pre);
	clear_Fbo_Rbo();


	brdfLutTex = new OGLTexture();

	glBindTexture(GL_TEXTURE_2D, brdfLutTex->GetObjectID());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	generate_bind_Fbo(captureFBO_lut);
	generate_bind_Rbo(captureRBO_lut);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, brdfLutTexSize, brdfLutTexSize);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLutTex->GetObjectID(), 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO_lut);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	clear_Fbo_Rbo();
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
	delete brdfLutShader;

}

void NCL::Rendering::OGLHdr::clearHDRBuffers()
{
	glDeleteFramebuffers(1, &this->captureFBO);
	glDeleteRenderbuffers(1, &this->captureRBO);
	glDeleteFramebuffers(1, &this->captureFBO_irr);
	glDeleteRenderbuffers(1, &this->captureRBO_irr);
	glDeleteFramebuffers(1, &this->captureFBO_pre);
	glDeleteRenderbuffers(1, &this->captureRBO_pre);
	glDeleteFramebuffers(1, &this->captureFBO_lut);//todo::test
	glDeleteRenderbuffers(1, &this->captureRBO_lut);
}

