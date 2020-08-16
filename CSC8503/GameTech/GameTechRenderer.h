#pragma once
#include "../../Plugins/OpenGLRendering/OGLRenderer.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../CSC8503Common/GameWorld.h"
#include "../../Plugins/OpenGLRendering/OGLLight.h"
#include "../../Plugins/OpenGLRendering/OGLHdr.h"
#include "../../Plugins/OpenGLRendering/OGLPosCamera.h"


namespace NCL {
	class Maths::Vector3;
	class Maths::Vector4;
	namespace CSC8503 {
		class RenderObject;

		class GameTechRenderer : public OGLRenderer {
		public:
			GameTechRenderer(GameWorld& world);
			~GameTechRenderer();
			vector<OGLLight*> lightArry;

			void setupHDR(OGLHdr* HdrEnv);
			bool isUsedPBR;
			bool isUsedCamPos;
			bool isUsedDov;
			bool isUsedMSAA;
			//	OGLShader* CompareShader;
			Camera* gameWorldCamera;

			OGLTexture* tempTex;
		protected:
			void RenderFrame()	override;
			void RendercameraFrame();

			OGLShader* defaultShader;

			GameWorld& gameWorld;

			void BuildObjectList();
			void SortObjectList();
			void RenderShadowMap();
			void RenderCamera();
			void CaculateViewPorjMat();
			void RenderDOVCamera();
			void caculateDovCamera();

			void drawQuad(OGLShader* shader);

			void drawFullScreenQuad(OGLShader* shader, OGLTexture* tex);
			void drawFullScreenQuad(OGLShader* shader, OGLTexture* mutiTex, int sampleN);

			void drawPosFullScreenQuad(OGLShader* shader, OGLTexture* tex, int IsUseLensDistortion, int IsUseFieldOfDepth);
			


			void SetupDebugMatrix(OGLShader* s) override;

			vector<const RenderObject*> activeObjects;


			OGLShader* shadowShader;
			GLuint		shadowFBO;
			Matrix4     shadowMatrix;
			OGLTexture* shadowTex;

			OGLPosCamera* posCamera;



			void RenderHDRSkybox(OGLTexture* cubeTexture, int glActiveTextureNum);
			void RenderHDRtoCubemap();
			void RenderCubemaptoIrradianceMap();
			void RenderPerFilterMap();
			void RenderBrdfLutMap();
			void ClearHDRBuffers();

			void DrawHDRCube(OGLShader* shader, OGLTexture* tex);
			OGLHdr* HdrEnv;

			GLenum status;

			int cubeTexture;

			float screenAspect;
			Matrix4 viewMatrix;
			Matrix4 projMatrix;



		};
	}
}

