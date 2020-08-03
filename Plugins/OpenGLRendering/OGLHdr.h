#pragma once
#include "stb_image.h"
#include "../../Common/TextureBase.h"
#include "OGLTexture.h"
#include "../../Common/TextureLoader.h"
#include <string>
#include <vector>
#include "Model.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "OGLFbo.h"
#include "OGLRenderer.h"

namespace NCL {
	namespace Rendering {
		class OGLHdr 
		{
		public:
			OGLHdr(std::string& HdrFilename);
			~OGLHdr();

			//void generateHdrCubeMaps();
			//void renderCube();

			OGLTexture* HdrTexture;
			OGLTexture* cubeTex;
			OGLTexture* irradianceMap;

			Model* cubeModel = nullptr;

			//OGLShader* HdrShader = nullptr;
			OGLShader* HdrToCubemapShader = nullptr;
			OGLShader* irradianceShader = nullptr;
			OGLShader* SkyboxShader = nullptr;


			unsigned int captureFBO;
			unsigned int captureRBO;

			int cubeTexSize;
			int cubeIrradianceTexSize;
			

		};
		
	}
}

