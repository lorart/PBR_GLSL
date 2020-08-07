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
			bool isUsedCamPos;
		//	OGLShader* CompareShader;
			Camera* gameWorldCamera;

			OGLTexture* tempTex ;
		protected:
			void RenderFrame()	override;
			void RendercameraFrame();

			OGLShader*		defaultShader;

			GameWorld&	gameWorld;

			void BuildObjectList();
			void SortObjectList();
			void RenderShadowMap();
			void RenderCamera(); 
			void CaculateViewPorjMat();
			void RenderDOVCamera();
			void caculateDovCamera();

			void drawFullScreenQuad(OGLShader* shader, OGLTexture* tex);
		
			

			void SetupDebugMatrix(OGLShader*s) override;

			vector<const RenderObject*> activeObjects;

			//shadow mapping things
			OGLShader*	shadowShader;
			//GLuint		shadowTex;
			GLuint		shadowFBO;
			Matrix4     shadowMatrix;


			OGLShader* ScreenQuadShader;
			OGLShader* cameraDovPosShader;
			OGLTexture* cameraBufferTex[2];
			OGLTexture* cameraDepBufferTex;
			

			OGLTexture* shadowTex;
			GLuint		cameraFBO;
			GLuint		cameraMsaa_FBO;
			GLuint		cameraMsaa_COL_RBO;
			GLuint		cameraMsaa_DEP_RBO;
			GLuint		cameraPosFBO;
		
			Model* ScreenQuad = nullptr;

			float screenAspect ;
			Matrix4 viewMatrix ;
			Matrix4 projMatrix;

			
			void RenderHDRSkybox(OGLTexture* cubeTexture, int glActiveTextureNum);
			void RenderHDRtoCubemap();
			void RenderCubemaptoIrradianceMap();
			void ClearHDRBuffers();

			void DrawHDRCube(OGLShader* shader, OGLTexture* tex);
			OGLHdr* HdrEnv;

			int cubeTexture;
			
	



		};
	}
}

