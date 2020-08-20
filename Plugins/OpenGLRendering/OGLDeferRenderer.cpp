#include "OGLDeferRenderer.h"

NCL::Rendering::OGLDeferRenderer::OGLDeferRenderer()
{
	for (int i = 0; i < 2; i++)
	{
		//cameraBufferTex[i] = new OGLTexture();
		//glBindTexture(GL_TEXTURE_2D, cameraBufferTex[i]->GetObjectID());
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, currentWidth, currentHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);  //why nullptr
		//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


		//glBindTexture(GL_TEXTURE_2D, 0);
	}


}

NCL::Rendering::OGLDeferRenderer::~OGLDeferRenderer()
{

	delete vertexPositionTex;
	delete diffuseTex;
	delete normalTex;
	delete depthTex;
	delete lightTex;

	glDeleteFramebuffers(1, &this->deferRenderFBO);
	glDeleteFramebuffers(1, &this->lightFbo);

}
