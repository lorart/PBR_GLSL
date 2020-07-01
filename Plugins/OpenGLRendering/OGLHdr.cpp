#include "OGLHdr.h"

NCL::Rendering::OGLHdr::OGLHdr(std::string& HdrFilename)
{
	HdrTexture = (OGLTexture*)NCL::Rendering::OGLTexture::LinerRGBATextureFromFilename(HdrFilename);
	cubeTex = (OGLTexture*)NCL::Rendering::OGLTexture::AllocateCubeTexture();
	
	cubeModel= new Model(Assets::MESHDIR + "CUBE" + ".obj", 0);

	
	//todo:check
	HdrToCubemapShader = new OGLShader("HDR_toCubeMap_Vert.glsl", "HDR_toCubeMap_Frag.glsl");
	HdrShader = new OGLShader("HDR_Vert.glsl", "HDR_Frag.glsl");
	generate_bind_Fbo(captureFBO);
	generate_bind_Rbo(captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
	//clear_Fbo();
	//cubeModel->meshes[0]->material->matShader= HdrShader;
	
	
}

NCL::Rendering::OGLHdr::~OGLHdr()
{
	delete cubeModel;
	delete HdrTexture;
	delete HdrShader;
	delete HdrToCubemapShader;
}

//todo:delete
/*
void NCL::Rendering::OGLHdr::generateHdrCubeMaps()
{
//capture from six perspective 
//reference https://learnopengl.com/PBR/IBL/Diffuse-irradiance
	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};


	HdrToCubemapShader->use();
	HdrToCubemapShader->setInt("mainTex", 0);
	HdrToCubemapShader->setMat4("projMatrix", captureProjection);
	
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, HdrTexture->GetObjectID());

	//todo::check
	glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions. 
							
	generate_bind_Fbo(captureFBO);//todo::check
	for (unsigned int i = 0; i < 6; ++i)
	{
		HdrToCubemapShader->setMat4("viewMatrix", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubeTex->GetObjectID(), 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderCube();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	


}

void NCL::Rendering::OGLHdr::renderCube()
{

}
*/