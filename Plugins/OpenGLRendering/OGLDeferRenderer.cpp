#include "OGLDeferRenderer.h"

NCL::Rendering::OGLDeferRenderer::OGLDeferRenderer(int currentWidth, int currentHeight)
{
	GbufferShader = new OGLShader("Gbuffer_Vert.glsl","Gbuffer_Frag.glsl");


	for (int i = 0; i < DefferTex::MAX_TEX; i++)
	{
		deferTexArry[i] = new OGLTexture();
		glBindTexture(GL_TEXTURE_2D, deferTexArry[i]->GetObjectID());
		if (i == DefferTex::NORMAL_DEPTH_TEX) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, currentWidth, currentHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);  
		}
		else {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, currentWidth, currentHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		}
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


		glBindTexture(GL_TEXTURE_2D, 0);
	}
	glGenFramebuffers(1, &deferRenderFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, deferRenderFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, deferTexArry[DefferTex::DIFFUSE_TEX]->GetObjectID(), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, deferTexArry[DefferTex::VERTEX_POSITION_TEX]->GetObjectID(), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, deferTexArry[DefferTex::NORMAL_DEPTH_TEX]->GetObjectID(), 0);

	glGenRenderbuffers(1, &deferRenderRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, deferRenderRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH32F_STENCIL8, currentWidth, currentWidth);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, deferRenderRBO);
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		/*if (status = GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) {
			std::cout << 1 << std::endl;
		}
		else if (status = GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT)
		{
			std::cout << 2 << std::endl;
		}
		else if (status = GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER)
		{
			std::cout << 3 << std::endl;
		}

		else if (status = GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER)
		{
			std::cout << 4 << std::endl;
		}

		else if (status = GL_FRAMEBUFFER_UNSUPPORTED)
		{
			std::cout << 5 << std::endl;
		}*/
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER,0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);



}

NCL::Rendering::OGLDeferRenderer::~OGLDeferRenderer()
{

	for (auto &i:deferTexArry)
	{
		delete i;
	}
	delete GbufferShader;
	glDeleteFramebuffers(1, &this->deferRenderFBO);
	glDeleteFramebuffers(1, &this->lightFbo);

}
