#pragma once
#include "stb_image.h"
#include "../../Common/TextureBase.h"
#include "OGLTexture.h"
#include "../../Common/TextureLoader.h"
#include <string>
#include <vector>
#include "Model.h"
namespace NCL {
	namespace Rendering {
		class OGLHdr 
		{
		public:
			OGLHdr(std::string& HdrFilename);
			~OGLHdr();

			OGLTexture* HdrTexture;
			Model* cubeModel=nullptr;
			OGLShader* HdrShader = nullptr;
			
			

		};
		
	}
}

