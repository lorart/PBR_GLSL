#pragma once
#include "OGLTexture.h"
#include "OGLRenderer.h"
#include "../../Common/TextureLoader.h"

using namespace NCL;
using namespace NCL::Rendering;

namespace NCL {
	namespace Rendering {
		void generateFBO_depth(GLuint texId, GLuint FboId,int texSize) {
			glGenTextures(1, &texId);
			glBindTexture(GL_TEXTURE_2D, texId);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
				texSize, texSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
			glBindTexture(GL_TEXTURE_2D, 0);

			glGenFramebuffers(1, &FboId);
			glBindFramebuffer(GL_FRAMEBUFFER, FboId);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texId, 0);
			glDrawBuffer(GL_NONE);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			glClearColor(1, 1, 1, 1);
		}


	}
}