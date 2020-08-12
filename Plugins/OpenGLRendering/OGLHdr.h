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
			OGLTexture* prefilterMap;
			OGLTexture* brdfLutTex;

			Model* cubeModel = nullptr;

			//OGLShader* HdrShader = nullptr;
			OGLShader* HdrToCubemapShader = nullptr;
			OGLShader* irradianceShader = nullptr;
			OGLShader* SkyboxShader = nullptr;
			OGLShader* prefilterShader = nullptr;
			OGLShader* brdfLutShader = nullptr;



			unsigned int captureFBO;
			unsigned int captureRBO;

			unsigned int captureFBO_irr;
			unsigned int captureRBO_irr;

			unsigned int captureFBO_pre;
			unsigned int captureRBO_pre;

			unsigned int captureFBO_lut;
			unsigned int captureRBO_lut;

			int cubeTexSize;
			int cubeIrradianceTexSize;
			int prefilterMapTexSize;
			int brdfLutTexSize;

		};
		
	}
}

