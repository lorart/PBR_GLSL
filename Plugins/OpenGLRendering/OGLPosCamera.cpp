#include "OGLPosCamera.h"

NCL::Rendering::OGLPosCamera::OGLPosCamera(int currentWidth, int currentHeight)
{
	this->currentWidth = currentWidth;
	this->currentHeight = currentHeight;
	this->mutiNum = 8;

	//ScreenQuad = new Model(Assets::MESHDIR + "PLANE" + ".obj", 0);

	ScreenQuad = new Model(Assets::MESHDIR + "PLANE1" + ".obj", 0);
	cameraDovPosShader = new OGLShader("ScreenQuad_Vert.glsl", "cameraDov_post_Frag.glsl");
	ScreenQuadShader = new OGLShader("ScreenQuad_Vert.glsl", "ScreenQuad_Frag.glsl");

	for (int i = 0; i < 2; i++)
	{
		cameraBufferTex[i] = new OGLTexture();
		glBindTexture(GL_TEXTURE_2D, cameraBufferTex[i]->GetObjectID());
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, currentWidth, currentHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);  //why nullptr
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glGenFramebuffers(1, &cameraFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, cameraFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, cameraBufferTex[1]->GetObjectID(), 0);

	/************/
	cameraDepBufferTex = new OGLTexture();
	glBindTexture(GL_TEXTURE_2D, cameraDepBufferTex->GetObjectID());
	//test
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, currentWidth, currentHeight, 0,
		GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, cameraDepBufferTex->GetObjectID(), 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		/*std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		if (status = GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) {
				std::cout << 1<< std::endl;
			}
			else if(status = GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT)
			{
				std::cout << 2 << std::endl;
			}
			else if (status = GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER)
			{
				std::cout << 3 << std::endl;
			}

			else if (status = GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER)
			{
				std::cout << 4<< std::endl;
			}

			else if (status = GL_FRAMEBUFFER_UNSUPPORTED)
			{
				std::cout << 5<< std::endl;
			}*/
	}
	//glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(1, 1, 1, 1);

	/******************/
	glGenFramebuffers(1, &cameraPosFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, cameraPosFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, cameraBufferTex[2]->GetObjectID(), 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	/******************/

	glGenFramebuffers(1, &cameraMsaa_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, cameraMsaa_FBO);
	//todo:
	cameraMsaa_mutiTex_col = new OGLTexture();
	glBindFramebuffer(GL_FRAMEBUFFER, cameraMsaa_FBO);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, cameraMsaa_mutiTex_col->GetObjectID());
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, mutiNum, GL_RGB8, currentWidth, currentHeight, GL_TRUE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, cameraMsaa_mutiTex_col->GetObjectID(), 0);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	cameraMsaa_mutiTex_dep = new OGLTexture();
	glBindFramebuffer(GL_FRAMEBUFFER, cameraMsaa_FBO);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, cameraMsaa_mutiTex_dep->GetObjectID());
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, mutiNum, GL_DEPTH_COMPONENT24, currentWidth, currentHeight, GL_TRUE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, cameraMsaa_mutiTex_dep->GetObjectID(), 0);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);


	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		//if (status = GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) {
		//	std::cout << 1 << std::endl;
		//}
		//else if (status = GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT)
		//{
		//	std::cout << 2 << std::endl;
		//}
		//else if (status = GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER)
		//{
		//	std::cout << 3 << std::endl;
		//}

		//else if (status = GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER)
		//{
		//	std::cout << 4 << std::endl;
		//}

		//else if (status = GL_FRAMEBUFFER_UNSUPPORTED)
		//{
		//	std::cout << 5 << std::endl;
		//}
	}


	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

NCL::Rendering::OGLPosCamera::~OGLPosCamera()
{
	delete cameraBufferTex;

	glDeleteFramebuffers(1, &cameraFBO);
	glDeleteFramebuffers(1, &cameraPosFBO);
	glDeleteFramebuffers(1, &cameraMsaa_FBO);



	delete cameraDovPosShader;
	delete ScreenQuadShader;
	delete cameraDepBufferTex;
	delete cameraDepBufferTex;
	delete ScreenQuad;
	delete cameraMsaa_mutiTex_col;
	delete cameraMsaa_mutiTex_dep;
}


