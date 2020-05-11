#pragma once
#include "OGLShader.h"
namespace NCL {
	namespace Rendering {
		class PBRshader :public OGLShader
		{

		public:
			int albedoValue;
			int normalValue;
			int metallicValue;
			int roughnessValue;
			int aoValue;

		};

	}
}