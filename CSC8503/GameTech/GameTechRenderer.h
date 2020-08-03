#pragma once
#include "../../Plugins/OpenGLRendering/OGLRenderer.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../CSC8503Common/GameWorld.h"
#include "../../Plugins/OpenGLRendering/OGLLight.h"
#include "../../Plugins/OpenGLRendering/OGLHdr.h"


namespace NCL {
	class Maths::Vector3;
	class Maths::Vector4;
	namespace CSC8503 {
		class RenderObject;

		class GameTechRenderer : public OGLRenderer	{
		public:
			GameTechRenderer(GameWorld& world);
			~GameTechRenderer();
			vector<OGLLight*> lightArry;
		
			void setupHDR(OGLHdr* HdrEnv);
			bool isUsedPBR;
			OGLShader* CompareShader;


		protected:
			void RenderFrame()	override;

			OGLShader*		defaultShader;

			GameWorld&	gameWorld;

			void BuildObjectList();
			void SortObjectList();
			void RenderShadowMap();
			void RenderCamera(); 
			void CaculateViewPorjMat();
			

			void SetupDebugMatrix(OGLShader*s) override;

			vector<const RenderObject*> activeObjects;

			//shadow mapping things
			OGLShader*	shadowShader;
			GLuint		shadowTex;
			GLuint		shadowFBO;
			Matrix4     shadowMatrix;


			float screenAspect ;
			Matrix4 viewMatrix ;
			Matrix4 projMatrix;

			
			void RenderHDRSkybox();
			void RenderHDRtoCubemap();
			void RenderCubemaptoIrradianceMap();
			void DrawHDRCube(OGLShader* shader, OGLTexture* tex);
			OGLHdr* HdrEnv;
			
	



		};
	}
}

