#pragma once
#include "../../Common/Matrix4.h"
#include "../../Common/TextureBase.h"
#include "../../Common/ShaderBase.h"
#include "../../Common/Vector4.h"
#include "../Plugins/OpenGLRendering/OGLMesh.h"
//#include "OGLMesh.h"

namespace NCL {
	using namespace NCL::Rendering;

	//class MeshGeometry;
	class OGLMesh;
	namespace CSC8503 {
		class Transform;
		using namespace Maths;

		class RenderObject
		{
			
		public:
			RenderObject(Transform* parentTransform, NCL::Rendering::OGLMesh* mesh, TextureBase* tex, ShaderBase* shader);
			~RenderObject();

			void SetDefaultTexture(TextureBase* t) {
				texture = t;
			}

			TextureBase* GetDefaultTexture() const {
				return texture;
			}

			NCL::Rendering::OGLMesh*	GetMesh() const {
				return mesh;
			}

			Transform*		GetTransform() const {
				return transform;
			}

			ShaderBase*		GetShader() const {
				return shader;
			}

			void SetColour(const Vector4& c) {
				colour = c;
			}

			Vector4 GetColour() const {
				return colour;
			}

		protected:
			NCL::Rendering::OGLMesh*	mesh;
			TextureBase*	texture;
			ShaderBase*		shader;
			Transform*		transform;
			Vector4			colour;
		};
	}
}
