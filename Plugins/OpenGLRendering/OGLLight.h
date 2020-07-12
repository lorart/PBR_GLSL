#pragma once
#include <vector>
#include "..\..\Common\Vector2.h"
#include "..\..\Common\Vector3.h"
#include "..\..\Common\Vector4.h"
using namespace NCL::Maths;
namespace NCL {
	namespace Rendering {
		class OGLLight
		{public:
			OGLLight();
			OGLLight(Vector4 lightColour, float	lightRadius, Vector3 lightPosition);
			~OGLLight();
		
			Vector4		lightColour;
			float		lightRadius;
			Vector3		lightPosition;
		
		};
	}
}

