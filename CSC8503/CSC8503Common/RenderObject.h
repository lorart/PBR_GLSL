#pragma once
#include "../../Common/Matrix4.h"
#include "../../Common/TextureBase.h"
#include "../../Common/ShaderBase.h"
#include "../../Common/Vector4.h"
#include "..\..\Plugins\OpenGLRendering\OGLMesh.h"
#include "..\..\Plugins\OpenGLRendering\OGLMaterial.h"




namespace NCL {
	using namespace NCL::Rendering;

	//class MeshGeometry;
	//class OGLMesh;
	namespace CSC8503 {
		class Transform;
		using namespace Maths;
	

		class RenderObject
		{
			
		public:

			RenderObject(Transform* parentTransform, OGLMesh* mesh, TextureBase* tex, ShaderBase* shader);

			RenderObject(Transform* parentTransform, OGLMesh* mesh, OGLMaterial* meterial, ShaderBase* pbrShader, bool isPBR);

			

			~RenderObject();

			void SetDefaultTexture(TextureBase* t) {
				texture = t;
			}

			TextureBase* GetDefaultTexture() const {	
				return texture;
			}

			OGLMesh*	GetMesh() const {
				return mesh;
			}

			Transform*		GetTransform() const {
				return transform;
			}

			ShaderBase*		GetShader() const {
				return shader;
			}
			void SetShader(ShaderBase* shadernew) {
				shader=shadernew;
			}

			void SetColour(const Vector4& c) {
				colour = c;
			}

			Vector4 GetColour() const {
				return colour;
			}
			bool GetIsPBR() const {
				return IsPBR;
			}
		
			OGLMaterial* GetOGLMaterial() const {
				return material;
			}
		protected:
			OGLMesh*	mesh;
			TextureBase*	texture;
			OGLMaterial* material;
			ShaderBase*		shader;
			Transform*		transform;
			Vector4			colour;
			bool IsPBR;
		};
	}
}
