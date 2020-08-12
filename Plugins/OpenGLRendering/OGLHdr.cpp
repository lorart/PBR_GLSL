#include "OGLHdr.h"

NCL::Rendering::OGLHdr::OGLHdr(std::string& HdrFilename)
{
	HdrTexture = (OGLTexture*)NCL::Rendering::OGLTexture::LinerRGBATextureFromFilename(HdrFilename);

	cubeTexSize = 512;
	cubeIrradianceTexSize = 32;
	prefilterMapTexSize = 128;
	brdfLutTexSize = 512;


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

	//generate_bind_Fbo(captureFBO_pre);
	//generate_bind_Rbo(captureRBO_pre);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, prefilterMapTexSize, prefilterMapTexSize);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO_pre);
	//clear_Fbo_Rbo();


	brdfLutTex = new OGLTexture();
	// pre-allocate enough memory for the LUT texture.
	glBindTexture(GL_TEXTURE_2D, brdfLutTex->GetObjectID());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
	// be sure to set wrapping mode to GL_CLAMP_TO_EDGE
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	// then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLutTex->GetObjectID(), 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	
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

