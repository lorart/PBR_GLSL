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

namespace NCL {
	namespace Rendering {
		class OGLDeferRenderer
		{
		public:
			OGLDeferRenderer();
			~OGLDeferRenderer();

			OGLTexture* vertexPositionTex;
			OGLTexture* diffuseTex;
			OGLTexture* normalTex;
			OGLTexture* depthTex;

			GLuint deferRenderFBO;


			OGLTexture* lightTex;
			GLuint lightFbo;



		
		};
	}
}

