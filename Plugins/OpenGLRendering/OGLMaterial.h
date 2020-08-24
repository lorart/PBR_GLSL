#pragma once
#include <vector>
#include "../../Common/MeshGeometry.h"
#include "..\..\Plugins\OpenGLRendering\OGLTexture.h"
#include "..\..\Plugins\OpenGLRendering\OGLShader.h"

namespace NCL {
	namespace Rendering {
		class OGLMaterial
		{
		public:
			OGLMaterial(std::vector<OGLTexture*> pbrTexArry, Vector3 albedoValue, Vector3 normalValue, float metallicValue, float roughnessValue);
			OGLMaterial(std::vector<OGLTexture*> pbrTexArry);
			~OGLMaterial();
			std::vector<OGLTexture*> pbrTexArry;
			Vector3 albedoValue = Vector3(0, 0, 0);
			Vector3 normalValue = Vector3(0, 0, 0);
			float metallicValue = 0;
			float roughnessValue = 0;
			float aoValue = 0;

			
			 int isUseClearcoat;
			 float clearCoat;
			 float clearCoatPerceptualRoughness;



			OGLShader* matShader;
			OGLShader* FengShader;
		};
	}
}
