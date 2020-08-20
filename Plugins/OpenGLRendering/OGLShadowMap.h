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
		class OGLShadowMap
		{
		public:
			OGLShadowMap(int SHADOWSIZE);
			~OGLShadowMap();

			OGLShader* shadowShader;
			OGLTexture* shadowTex;
			GLuint shadowFBO;
			Matrix4     shadowMatrix;
		};
	}
}

