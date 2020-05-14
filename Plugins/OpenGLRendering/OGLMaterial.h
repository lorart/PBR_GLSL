#pragma once
#include <vector>
#include "../../Common/MeshGeometry.h"
#include "..\..\Plugins\OpenGLRendering\OGLTexture.h"

namespace NCL {
	namespace Rendering {
		class OGLMaterial
		{
		public:
			OGLMaterial(std::vector<OGLTexture*> pbrTexArry, Vector3 albedoValue, Vector3 normalValue, int metallicValue, int roughnessValue, int aoValue);
			OGLMaterial(std::vector<OGLTexture*> pbrTexArry);
			~OGLMaterial();
			std::vector<OGLTexture*> pbrTexArry;
			Vector3 albedoValue = Vector3(0, 0, 0);
			Vector3 normalValue = Vector3(0, 0, 0);
			int metallicValue = 0;
			int roughnessValue = 0;
			int aoValue = 0;
		};
	}
}
