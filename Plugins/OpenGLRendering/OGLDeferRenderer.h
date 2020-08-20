#pragma once
#include "../../Common/TextureBase.h"
#include "OGLTexture.h"
#include "../../Common/TextureLoader.h"
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "OGLFbo.h"
#include "OGLRenderer.h"
#include "OGLShader.h"
namespace NCL {
	namespace Rendering {
		enum DefferTex
		{
			VERTEX_POSITION_TEX,
			DIFFUSE_TEX,
			NORMAL_DEPTH_TEX,
			LIGHT_TEX,
			MAX_TEX

		}; 

		
		class OGLDeferRenderer
		{
		public:
			OGLDeferRenderer(int currentWidth, int currentHeight);
			~OGLDeferRenderer();

			/*	OGLTexture* vertexPositionTex;
				OGLTexture* diffuseTex;
				OGLTexture* normalTex;
				OGLTexture* depthTex;*/

			OGLTexture* deferTexArry[MAX_TEX];

			GLuint deferRenderFBO;
			GLuint lightFbo;
			GLuint deferRenderRBO;

			OGLShader* GbufferShader;


		
		};
	}
}

