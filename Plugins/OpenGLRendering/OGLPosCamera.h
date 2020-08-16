
#pragma once
#include "../../Common/TextureBase.h"
#include "glad\glad.h"

#include <string>
#include <vector>
#include "OGLShader.h"
#include "OGLTexture.h"
#include "OGLRenderer.h"
#include "Model.h"
#include "..\..\Common\Camera.h"

namespace NCL {
	namespace Rendering {
		class OGLPosCamera
		{
		public:
			OGLPosCamera(int currentWidth, int currentHeight,Camera* gameCamera);
			~OGLPosCamera();
			
			int currentWidth;
			int currentHeight;
			Model* ScreenQuad = nullptr;


			OGLShader* ScreenQuadShader;
			OGLShader* cameraDovShader;
			OGLShader* cameraDistortionShader;
			OGLTexture* cameraBufferTex[2];
			OGLTexture* cameraDepBufferTex;
			GLuint		cameraFBO;
			GLuint		cameraMsaa_FBO;
			OGLTexture* cameraMsaa_mutiTex_col;
			OGLTexture* cameraMsaa_mutiTex_dep;


			GLuint		cameraPosFBO;
			GLuint		mutiNum;



			//float screenAspect;
			//Matrix4 viewMatrix;
			//Matrix4 projMatrix;


		};

	}
}
