#pragma once
#include "OGLTexture.h"
#include "OGLRenderer.h"
#include "../../Common/TextureLoader.h"

using namespace NCL;
using namespace NCL::Rendering;

namespace NCL {
	namespace Rendering {
		void generateFboAndTex_bind_depth(GLuint texId, GLuint fboId,int texSize) {
			glGenTextures(1, &texId);
			glBindTexture(GL_TEXTURE_2D, texId);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
				texSize, texSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
			glBindTexture(GL_TEXTURE_2D, 0);

			glGenFramebuffers(1, &fboId);
			glBindFramebuffer(GL_FRAMEBUFFER, fboId);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texId, 0);
			glDrawBuffer(GL_NONE);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			glClearColor(1, 1, 1, 1);//todo:check
		}
	
	
		void genBinTex_CubeMap(GLuint cubemapId) {
			glGenTextures(1, &cubemapId);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapId);
		}
	
		void generate_bind_Fbo(GLuint fboId) {
			glGenFramebuffers(1, &fboId);
			glBindFramebuffer(GL_FRAMEBUFFER, fboId);
		}
		void clear_Fbo() {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		void generate_bind_Rbo(GLuint rboId) {
			glGenRenderbuffers(1, &rboId);
			glBindRenderbuffer(GL_RENDERBUFFER, rboId);
		}
		void bindFboRbo_depth(GLuint fboId, GLuint rboId, int texSize) {
		

			
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboId);
		}


	}
}