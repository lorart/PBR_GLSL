#pragma once
#include "../../Common/Matrix4.h"
#include "../../Common/TextureBase.h"
#include "../../Common/ShaderBase.h"
#include "../../Common/Vector4.h"
#include "..\..\Plugins\OpenGLRendering\OGLMesh.h"
#include "..\..\Plugins\OpenGLRendering\OGLTexture.h"


namespace NCL {
	using namespace NCL::Rendering;
	
	//class MeshGeometry;
	//class OGLMesh;
	namespace CSC8503 {
		class Transform;
		using namespace Maths;
		enum TextureType {
			ALBEDO_MAP,
			NORMAL_MAP,
			METALLIC_MAP,
			ROUGHNESS_MAP,
			AO_MAP
		};

		class RenderObject
		{
			
		public:

			RenderObject(Transform* parentTransform, OGLMesh* mesh, TextureBase* tex, ShaderBase* shader);

			RenderObject(Transform* parentTransform, OGLMesh* mesh, vector<OGLTexture*> pbrTexArry, ShaderBase* shader,bool isPBR);

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

			void SetColour(const Vector4& c) {
				colour = c;
			}

			Vector4 GetColour() const {
				return colour;
			}
			bool GetIsPBR() const {
				return IsPBR;
			}
		
			vector<OGLTexture*> GetPbrTexArry() const
			{
				return pbrTexArry;
			}
		protected:
			OGLMesh*	mesh;
			TextureBase*	texture;
			vector<OGLTexture*> pbrTexArry;
			ShaderBase*		shader;
			Transform*		transform;
			Vector4			colour;
			bool IsPBR;
		};
	}
}
