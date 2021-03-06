#include "GameTechRenderer.h"
#include "../CSC8503Common/GameObject.h"
#include "../../Common/Camera.h"
#include "../../Common/Vector2.h"
#include "../../Common/Vector3.h"


using namespace NCL;
using namespace Rendering;
using namespace CSC8503;

#define SHADOWSIZE 4096


Matrix4 biasMatrix = Matrix4::Translation(Vector3(0.5, 0.5, 0.5)) * Matrix4::Scale(Vector3(0.5, 0.5, 0.5));

GameTechRenderer::GameTechRenderer(GameWorld& world) : OGLRenderer(*Window::GetWindow()), gameWorld(world) {
#pragma region shadowmap
	glEnable(GL_DEPTH_TEST);
	//todo

	//todo:check
	shadowShader = new OGLShader("GameTechShadowVert.glsl", "GameTechShadowFrag.glsl");

	/*glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);*/
	shadowTex = new OGLTexture();
	glBindTexture(GL_TEXTURE_2D, shadowTex->GetObjectID());
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex->GetObjectID(), 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

#pragma endregion

#pragma region  camera




	glBindTexture(GL_TEXTURE_2D, 0);
#pragma endregion

	isUsedPBR = true;
	isUsedCamPos = true;
	isUsedMSAA = true;
	isUsedDov= true;
	isUsedShadowMap = false;
	HdrEnv = nullptr;
	float screenAspect = 0;
	Matrix4 viewMatrix;
	Matrix4 projMatrix;


	cubeTexture = 0;


	gameWorldCamera = world.GetMainCamera();
	posCamera = new OGLPosCamera(currentWidth, currentHeight, gameWorldCamera);
	tempTex = (OGLTexture*)TextureLoader::LoadAPITexture("checkerboard.png");
}

GameTechRenderer::~GameTechRenderer() {

	delete posCamera;
	//	glDeleteTextures(1, &shadowTex);
	delete shadowTex;
	glDeleteFramebuffers(1, &shadowFBO);
	//todo:check
	delete HdrEnv;


	delete tempTex;

	for (auto& i : lightArry) {
		delete i;
	}

}

void GameTechRenderer::RenderFrame() {
	CaculateViewPorjMat();
	glEnable(GL_CULL_FACE);
//	glClearColor(1, 1, 1, 1);


	RenderDOVCamera();


	glDisable(GL_CULL_FACE); //Todo - text indices are going the wrong way...
	//TODO: 
}

void GameTechRenderer::BuildObjectList() {
	std::vector<GameObject*>::const_iterator first;
	std::vector<GameObject*>::const_iterator last;

	gameWorld.GetObjectIterators(first, last);

	activeObjects.clear();

	for (std::vector<GameObject*>::const_iterator i = first; i != last; ++i) {
		if ((*i)->IsActive()) {
			const RenderObject* g = (*i)->GetRenderObject();
			if (g) {
				activeObjects.emplace_back(g);
			}
		}
	}
}

void GameTechRenderer::SortObjectList() {

}

void GameTechRenderer::RenderShadowMap() {

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);

	glCullFace(GL_FRONT);

	//todo::test
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(-1.5f, -2.0f);

	BindShader(shadowShader);
	int mvpLocation = glGetUniformLocation(shadowShader->GetProgramID(), "mvpMatrix");

	//todo:check***********
	Matrix4 shadowViewMatrix = Matrix4::BuildViewMatrix(lightArry[0]->lightPosition, Vector3(0, 0, 0), Vector3(0, 1, 0));
	//Matrix4 shadowViewMatrix = Matrix4::BuildViewMatrix(Vector3(0, 0, 0), Vector3(0, 0, 0), Vector3(0, 1, 0));
	Matrix4 shadowProjMatrix = Matrix4::Perspective(150.0f, 10000.0f, 1, 45.0f);

	Matrix4 mvMatrix = shadowProjMatrix * shadowViewMatrix;

	shadowMatrix = biasMatrix * mvMatrix; //we'll use this one later on

	for (const auto& i : activeObjects) {
		Matrix4 modelMatrix = (*i).GetTransform()->GetWorldMatrix();
		Matrix4 mvpMatrix = mvMatrix * modelMatrix;
		glUniformMatrix4fv(mvpLocation, 1, false, (float*)&mvpMatrix);
		BindMesh((*i).GetMesh());
		DrawBoundMesh();
	}

	//todo::test




	glViewport(0, 0, currentWidth, currentHeight);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);



	glCullFace(GL_BACK);
}


void GameTechRenderer::RenderCamera() {
	//float screenAspect = (float)currentWidth / (float)currentHeight;
	//Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	//Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);

	OGLShader* activeShader = nullptr;
	int projLocation = 0;
	int viewLocation = 0;
	int modelLocation = 0;
	//int colourLocation  = 0;
	//int hasVColLocation = 0;
	int hasTexLocation = 0;
	int shadowLocation = 0;

	int lightPosLocation = 0;
	int lightColourLocation = 0;
	int lightRadiusLocation = 0;

	int albedoValueLocation = 0;
	int normalValueLocation = 0;
	int metallicValueLocation = 0;
	int roughnessValueLocation = 0;
	int aoValueLocation = 0;

	int cameraLocation = 0;

	//TODO - PUT IN FUNCTION
	glActiveTexture(GL_TEXTURE0 + 14);
	glBindTexture(GL_TEXTURE_2D, shadowTex->GetObjectID());

	//for (const auto& i : activeObjects) {
	for (size_t l = 0; l < activeObjects.size(); l++)
	{
		OGLShader* shader;
		const auto i = activeObjects[l];

		if (isUsedPBR) {
			// shader = (OGLShader*)(*i).GetShader();
				// shader = CompareShader;
			shader = i->GetOGLMaterial()->matShader;
		}
		else {
			shader = i->GetOGLMaterial()->FengShader;
		}



		//OGLShader* shader = (OGLShader*)(*i).GetShader();

		BindShader(shader);
		//todo:why
		if (activeShader != shader) {
			if ((*i).GetIsPBR()) {

				//if((*i).GetPbrTexArry().size()!=5){
				auto tempMaterial = (*i).GetOGLMaterial();
				if (tempMaterial->pbrTexArry.size() != 5) {
					std::cout << "error: losing Pbr Texture!" << std::endl;
				}
				else {

					OGLTexture* tempAlbedo = (OGLTexture*)tempMaterial->pbrTexArry[TextureType::ALBEDO_MAP];
					OGLTexture* tempNormal = (OGLTexture*)tempMaterial->pbrTexArry[TextureType::NORMAL_MAP];
					OGLTexture* tempMetallic = (OGLTexture*)tempMaterial->pbrTexArry[TextureType::METALLIC_MAP];
					OGLTexture* tempRoughness = (OGLTexture*)tempMaterial->pbrTexArry[TextureType::ROUGHNESS_MAP];
					OGLTexture* tempAO = (OGLTexture*)tempMaterial->pbrTexArry[TextureType::AO_MAP];

					//std::cout << "tempAlbedo->GetObjectID() "<<tempAlbedo->GetObjectID() << std:: endl;

					BindTextureToShader(tempAlbedo, "albedo_map", 0);
					BindTextureToShader(tempNormal, "normal_map", 1);
					BindTextureToShader(tempMetallic, "metallic_map", 2);
					BindTextureToShader(tempRoughness, "roughness_map", 3);
					BindTextureToShader(tempAO, "ao_map", 4);

					glActiveTexture(GL_TEXTURE0 + 5);
					glBindTexture(GL_TEXTURE_CUBE_MAP, HdrEnv->irradianceMap->GetObjectID());
					shader->setInt("irradianceMap",  5);

					glActiveTexture(GL_TEXTURE0 + 6);
					glBindTexture(GL_TEXTURE_CUBE_MAP, HdrEnv->prefilterMap->GetObjectID());
					shader->setInt("prefilterMap", 6);

					BindTextureToShader(HdrEnv->brdfLutTex_Load, "brdfLUT", 7);
				}



				/*	glUniform3fv(albedoValueLocation, 1, (float*)&(tempMaterial->albedoValue));
					glUniform3fv(normalValueLocation, 1, (float*)&(tempMaterial->normalValue));
					glUniform1f(metallicValueLocation, tempMaterial->metallicValue);
					glUniform1f(roughnessValueLocation, tempMaterial->roughnessValue);
					glUniform1f(aoValueLocation, tempMaterial->aoValue);*/


			}
			else {
				auto tempMaterial = (*i).GetOGLMaterial();
				OGLTexture* tempAlbedo = (OGLTexture*)tempMaterial->pbrTexArry[TextureType::ALBEDO_MAP];
				BindTextureToShader(tempAlbedo, "mainTex", 6);
				//	BindTextureToShader((OGLTexture*)(*i).GetDefaultTexture(), "mainTex", 6);

				hasTexLocation = glGetUniformLocation(shader->GetProgramID(), "hasTexture");

			}
			//todo: make it better
			auto tempMaterial = (*i).GetOGLMaterial();
			if ((*i).GetIsPBR()) {
				//albedoValueLocation = glGetUniformLocation(shader->GetProgramID(), "albedoValue");
				metallicValueLocation = glGetUniformLocation(shader->GetProgramID(), "metallicValue");
				roughnessValueLocation = glGetUniformLocation(shader->GetProgramID(), "roughnessValue");
				aoValueLocation = glGetUniformLocation(shader->GetProgramID(), "aoValue");

				//glUniform3fv(albedoValueLocation, 1, (float*)&(tempMaterial->albedoValue));
				glUniform1f(metallicValueLocation, tempMaterial->metallicValue);
				glUniform1f(roughnessValueLocation, tempMaterial->roughnessValue);
				//todo:delete
				//std::cout << "tempMaterial->roughnessValue=" << tempMaterial->roughnessValue << std::endl;
				glUniform1f(aoValueLocation, tempMaterial->aoValue);

				//******************************
				//clear coat
				/*
				uniform int isUseClearcoat;
				uniform float Clearcoat;
				uniform float clearCoatPerceptualRoughness;*/
				shader->setInt("isUseClearcoat", tempMaterial->isUseClearcoat);
				shader->setFloat("clearCoat", tempMaterial->clearCoat);
				shader->setFloat("clearCoatPerceptualRoughness", tempMaterial->clearCoatPerceptualRoughness);


			}

			albedoValueLocation = glGetUniformLocation(shader->GetProgramID(), "albedoValue");
			glUniform3fv(albedoValueLocation, 1, (float*)&(tempMaterial->albedoValue));

			projLocation = glGetUniformLocation(shader->GetProgramID(), "projMatrix");
			viewLocation = glGetUniformLocation(shader->GetProgramID(), "viewMatrix");
			modelLocation = glGetUniformLocation(shader->GetProgramID(), "modelMatrix");
			shadowLocation = glGetUniformLocation(shader->GetProgramID(), "shadowMatrix");
			//	colourLocation  = glGetUniformLocation(shader->GetProgramID(), "objectColour");
			//	hasVColLocation = glGetUniformLocation(shader->GetProgramID(), "hasVertexColours");

			cameraLocation = glGetUniformLocation(shader->GetProgramID(), "cameraPos");
			glUniform3fv(cameraLocation, 1, (float*)&gameWorld.GetMainCamera()->GetPosition());

			glUniformMatrix4fv(projLocation, 1, false, (float*)&projMatrix);
			glUniformMatrix4fv(viewLocation, 1, false, (float*)&viewMatrix);


			lightPosLocation = glGetUniformLocation(shader->GetProgramID(), "lightPos");
			lightColourLocation = glGetUniformLocation(shader->GetProgramID(), "lightColour");
			lightRadiusLocation = glGetUniformLocation(shader->GetProgramID(), "lightRadius");




			int shadowTexLocation = glGetUniformLocation(shader->GetProgramID(), "shadowTex");
			glUniform1i(shadowTexLocation, 14);
			activeShader = shader;
		}

		//todo:check
		if (lightArry.size() > 0)
		{
			//todo:single light->more light

			glUniform3fv(lightPosLocation, 1, (float*)&(lightArry[0]->lightPosition));
			glUniform4fv(lightColourLocation, 1, (float*)&(lightArry[0]->lightColour));
			glUniform1f(lightRadiusLocation, lightArry[0]->lightRadius);
		}

		Matrix4 modelMatrix = (*i).GetTransform()->GetWorldMatrix();
		glUniformMatrix4fv(modelLocation, 1, false, (float*)&modelMatrix);

	
		if (isUsedShadowMap) {
			shader->setInt("isUsedShadowMap", 1);

			Matrix4 fullShadowMat = shadowMatrix * modelMatrix;
			glUniformMatrix4fv(shadowLocation, 1, false, (float*)&fullShadowMat);
		}
		else
		{
			shader->setInt("isUsedShadowMap", 0);
		}

		shader->setFloat("cameraFocusDistance",gameWorld.GetMainCamera()->focusDistance);
		shader->setFloat("lens", gameWorld.GetMainCamera()->getLens());

	

		//TODO:
		//glUniform4fv(colourLocation, 1, (float*)&i->GetColour());

		//glUniform1i(hasVColLocation,!(*i).GetMesh()->GetColourData().empty());
		if ((*i).GetIsPBR()) {

		}
		else {
			glUniform1i(hasTexLocation, (*i).GetDefaultTexture() ? 1 : 0);
		}


		BindMesh((*i).GetMesh());
		DrawBoundMesh();
		//todo::check
	//	glActiveTexture(GL_TEXTURE0);
	}

}


void GameTechRenderer::SetupDebugMatrix(OGLShader* s) {
	float screenAspect = (float)currentWidth / (float)currentHeight;
	Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);

	Matrix4 vp = projMatrix * viewMatrix;

	int matLocation = glGetUniformLocation(s->GetProgramID(), "viewProjMatrix");

	glUniformMatrix4fv(matLocation, 1, false, (float*)&vp);
}

void NCL::CSC8503::GameTechRenderer::setupHDR(OGLHdr* hdrEnv)
{
	this->HdrEnv = hdrEnv;

	RenderHDRtoCubemap();
	RenderCubemaptoIrradianceMap();
	RenderPerFilterMap();
	//RenderBrdfLutMap();

	//todo:delete
	ClearHDRBuffers();
}

void GameTechRenderer::RendercameraFrame()
{
	BuildObjectList();
	SortObjectList();
	RenderShadowMap();
	if (isUsedMSAA) {
		glBindFramebuffer(GL_FRAMEBUFFER, posCamera->cameraMsaa_FBO);
	}
	else {
		glBindFramebuffer(GL_FRAMEBUFFER, posCamera->cameraFBO);
	}

	//todo:delete
	//RenderCubemaptoIrradianceMap();
	//RenderPerFilterMap();
	//RenderBrdfLutMap();
	
	RenderCamera();
	


	//todo::error***************
	//todo::delete
   RenderHDRSkybox(HdrEnv->cubeTex,11);

}

void GameTechRenderer::drawQuad(OGLShader* shader)
{
	OGLTexture* tex = tempTex;
	Transform* quadtransform = new Transform();
	BindShader(shader);
	RenderObject* i = new RenderObject(quadtransform, posCamera->ScreenQuad->meshes[0], tex, shader);
	BindMesh((*i).GetMesh());
	DrawBoundMesh();
	
	
}



void GameTechRenderer::drawFullScreenQuad(OGLShader* shader, OGLTexture* tex)
{
	OGLShader* activeShader = nullptr;

	Transform* quadtransform = new Transform(); {
		projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
		viewMatrix = Matrix4();

		Matrix4 modelMatrix = Matrix4();

		modelMatrix.Rotation(90.0f, Vector3(1, 1, 1));

		//Transform* quadtransform = new Transform();


		BindShader(shader);
		int multyN = 0;

		shader->setMat4("modelMatrix", modelMatrix);
		shader->setMat4("viewMatrix", viewMatrix);
		shader->setMat4("projMatrix", projMatrix);
	}
	BindTextureToShader(tex, "mainTex", 0);
	


	//Transform* parentTransform, OGLMesh* mesh, TextureBase* colourtex, ShaderBase* shader
	RenderObject* i = new RenderObject(quadtransform, posCamera->ScreenQuad->meshes[0], tex, shader);
	BindMesh((*i).GetMesh());
	DrawBoundMesh();
	glUseProgram(0);
}


void GameTechRenderer::drawPosFullScreenQuad(OGLShader* shader, OGLTexture* tex,int IsUseLensDistortion,int IsUseFieldOfDepth)
{
	OGLShader* activeShader = nullptr;

	Transform* quadtransform = new Transform();
	if (activeShader != shader) {
		projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
		viewMatrix = Matrix4();

		Matrix4 modelMatrix = Matrix4();

		modelMatrix.Rotation(90.0f, Vector3(1, 1, 1));

		//Transform* quadtransform = new Transform();


		BindShader(shader);
		int multyN = 0;

		shader->setMat4("modelMatrix", modelMatrix);
		shader->setMat4("viewMatrix", viewMatrix);
		shader->setMat4("projMatrix", projMatrix);
		shader->setInt("texWide", currentWidth);
		shader->setInt("texHight", currentHeight);
		/*
		uniform int IsUseLensDistortion;
uniform int IsUseFieldOfDepth;
		*/
	}
	
	//BindTextureToShader(tex, "mainTex", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex->GetObjectID());
	shader->setInt("mainTex", 0);


	shader->setInt("lens", gameWorld.GetMainCamera()->getLens());
	shader->setInt("texHight", currentHeight);

	shader->setInt("IsUseLensDistortion", IsUseLensDistortion);
	shader->setInt("IsUseFieldOfDepth", IsUseFieldOfDepth);

	//Transform* parentTransform, OGLMesh* mesh, TextureBase* colourtex, ShaderBase* shader
	RenderObject* i = new RenderObject(quadtransform, posCamera->ScreenQuad->meshes[0], tex, shader);
	BindMesh((*i).GetMesh());
	DrawBoundMesh();
	activeShader = shader;
	glUseProgram(0);
}


void GameTechRenderer::drawFullScreenQuad(OGLShader* shader, OGLTexture* mutiTex, int sampleN)
{
	//Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix = Matrix4();

	Matrix4 modelMatrix = Matrix4();
	modelMatrix.Rotation(90.0f, Vector3(1, 1, 1));

	Transform* quadtransform = new Transform();


	BindShader(shader);

	shader->setMat4("modelMatrix", modelMatrix);
	shader->setMat4("viewMatrix", viewMatrix);
	shader->setMat4("projMatrix", projMatrix);

	//todo:check
	BindTextureToShader(mutiTex, "MutiTex", 0);
	shader->setInt("nMultiSample", sampleN);



	//Transform* parentTransform, OGLMesh* mesh, TextureBase* colourtex, ShaderBase* shader
	RenderObject* i = new RenderObject(quadtransform, posCamera->ScreenQuad->meshes[0], mutiTex, shader);
	BindMesh((*i).GetMesh());
	DrawBoundMesh();
}


void GameTechRenderer::caculateDovCamera()
{


}

void GameTechRenderer::RenderDOVCamera()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	if (isUsedMSAA) {
		glBindFramebuffer(GL_FRAMEBUFFER, posCamera->cameraMsaa_FBO);
		status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{

			std::cout << status << "   The frame buffer status is not complete!" << std::endl;
			return;
		}
		glEnable(GL_MULTISAMPLE);
	}
	else {

		glBindFramebuffer(GL_FRAMEBUFFER, posCamera->cameraFBO);
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	//glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glViewport(0, 0, currentWidth, currentHeight);

	RendercameraFrame();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
//	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	if (isUsedMSAA) {
    	//std::cout << "mutiSample" << std::endl;
		glBindFramebuffer(GL_READ_FRAMEBUFFER, posCamera->cameraMsaa_FBO);
		status = glCheckFramebufferStatus(GL_READ_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << status << "   The frame buffer status is not complete!" << std::endl;
			return;
		}


		glBindFramebuffer(GL_READ_FRAMEBUFFER, posCamera->cameraMsaa_FBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, posCamera->cameraFBO);


		status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "The frame buffer status is not complete!" << std::endl;
			return;
		}
		glBlitFramebuffer(0, 0, currentWidth, currentHeight,
			0, 0, currentWidth, currentHeight,
			GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glDisable(GL_MULTISAMPLE);


		//glBindFramebuffer(GL_FRAMEBUFFER, posCamera->cameraFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
	}

	//*****************************************
	glBindFramebuffer(GL_FRAMEBUFFER, posCamera->cameraDistortionFBO);
	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{

		std::cout << status << "   The frame buffer status is not complete!" << std::endl;

		if (status = GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) {
			std::cout << 1 << std::endl;
		}
		else if (status = GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT)
		{
			std::cout << 2 << std::endl;
		}
		else if (status = GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER)
		{
			std::cout << 3 << std::endl;
		}

		else if (status = GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER)
		{
			std::cout << 4 << std::endl;
		}

		else if (status = GL_FRAMEBUFFER_UNSUPPORTED)
		{
			std::cout << 5 << std::endl;
		}

		return;
	}
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
			if (isUsedDov) {
				drawPosFullScreenQuad(posCamera->cameraDovShader, posCamera->cameraBufferTex[0], 1, 1);//todo:
			}
			else {
				drawPosFullScreenQuad(posCamera->cameraDovShader, posCamera->cameraBufferTex[0], 1, 0);
			}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	//*****************************************
	if (isUsedCamPos)
	{
		drawPosFullScreenQuad(posCamera->cameraDistortionShader, posCamera->cameraBufferTex[1], 1, 0);
	}
	else
	{
		drawPosFullScreenQuad(posCamera->cameraDistortionShader, posCamera->cameraBufferTex[1], 0, 0);
	}
		


}
void NCL::CSC8503::GameTechRenderer::CaculateViewPorjMat()
{
	screenAspect = (float)currentWidth / (float)currentHeight;
	viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);

}


void NCL::CSC8503::GameTechRenderer::RenderHDRSkybox(OGLTexture* cubeTexture, int glActiveTextureNum)
{
	static int cubeTexNum = 0;
	glDepthFunc(GL_LEQUAL);


	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	//todo::test

	glActiveTexture(GL_TEXTURE0 + glActiveTextureNum);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTexture->GetObjectID());

	HdrEnv->SkyboxShader->use();
	HdrEnv->SkyboxShader->setMat4("projection", this->projMatrix);
	HdrEnv->SkyboxShader->setMat4("view", this->viewMatrix);

	HdrEnv->SkyboxShader->setInt("environmentMap", 0 + glActiveTextureNum);


	//DrawHDRCube(HdrEnv->SkyboxShader, HdrEnv->cubeTex);
	DrawHDRCube(HdrEnv->SkyboxShader, cubeTexture);



	glCullFace(GL_BACK);

}

void NCL::CSC8503::GameTechRenderer::RenderHDRtoCubemap()
{

	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	//todo::check
	HdrEnv->HdrToCubemapShader->use();
	HdrEnv->HdrToCubemapShader->setMat4("projection", captureProjection);
	HdrEnv->HdrToCubemapShader->setInt("hdrMap", 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, HdrEnv->HdrTexture->GetObjectID());

	glViewport(0, 0, HdrEnv->cubeTexSize, HdrEnv->cubeTexSize); // don't forget to configure the viewport to the capture dimensions. 
	//glViewport(0, 0, 512, 512);

	//glBindFramebuffer(GL_FRAMEBUFFER, HdrEnv->captureFBO);
	generate_bind_Fbo(HdrEnv->captureFBO);
	//std::cout << "captureFBO= " << HdrEnv->captureFBO<< std::endl;

	for (int i = 0; i < 6; i++)
	{
		HdrEnv->HdrToCubemapShader->setMat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, HdrEnv->cubeTex->GetObjectID(), 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		DrawHDRCube(HdrEnv->HdrToCubemapShader, HdrEnv->HdrTexture);
		//renderHDR to  cubemap

	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//	glDeleteFramebuffers(1, &HdrEnv->captureFBO);//todo::test

}

void NCL::CSC8503::GameTechRenderer::RenderCubemaptoIrradianceMap()
{

	glBindFramebuffer(GL_FRAMEBUFFER, HdrEnv->captureFBO_irr);
	glBindRenderbuffer(GL_RENDERBUFFER, HdrEnv->captureRBO_irr);
	
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};
	/*
	//todo:test
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, HdrEnv->cubeIrradianceTexSize,
		HdrEnv->cubeIrradianceTexSize);*/
	HdrEnv->irradianceShader->use();
	HdrEnv->irradianceShader->setInt("environmentMap", 9);
	HdrEnv->irradianceShader->setMat4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE9);
	//todo::check
	glBindTexture(GL_TEXTURE_CUBE_MAP, HdrEnv->cubeTex->GetObjectID());

	glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.

	//todo::test
	generate_bind_Fbo(HdrEnv->captureFBO_irr);


	for (unsigned int i = 0; i < 6; ++i)
	{
		HdrEnv->irradianceShader->setMat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, HdrEnv->irradianceMap->GetObjectID(), 0);
		//std::cout << "HdrEnv->irradianceMap->GetObjectID()= " << HdrEnv->irradianceMap->GetObjectID() << std::endl;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//todo::check

		DrawHDRCube(HdrEnv->irradianceShader, HdrEnv->HdrTexture);
		//DrawHDRCube(HdrEnv->HdrToCubemapShader, HdrEnv->HdrTexture);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void NCL::CSC8503::GameTechRenderer::RenderBrdfLutMap()
{
	glBindFramebuffer(GL_FRAMEBUFFER, HdrEnv->captureFBO_lut);
	glBindRenderbuffer(GL_RENDERBUFFER, HdrEnv->captureRBO_lut);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, HdrEnv->brdfLutTexSize, HdrEnv->brdfLutTexSize);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, HdrEnv->brdfLutTex->GetObjectID(), 0);

	glViewport(0, 0, 512, 512);
	HdrEnv->brdfLutShader->use();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawQuad(HdrEnv->brdfLutShader);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, currentWidth, currentHeight);
}

void GameTechRenderer::RenderPerFilterMap()
{
	clear_Fbo_Rbo();
	//generate_bind_Fbo(HdrEnv->captureFBO_irr);
	//generate_bind_Rbo(HdrEnv->captureRBO_irr);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, HdrEnv->cubeIrradianceTexSize, HdrEnv->cubeIrradianceTexSize);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, HdrEnv->captureRBO_irr);


	glBindFramebuffer(GL_FRAMEBUFFER, HdrEnv->captureFBO_pre);
	glBindRenderbuffer(GL_RENDERBUFFER, HdrEnv->captureRBO_pre);
	std::cout << "***HdrEnv->captureFBO_pre" << HdrEnv->captureFBO_pre << std::endl;
	std::cout << "***HdrEnv->captureRBO_pre" << HdrEnv->captureRBO_pre << std::endl;

	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	HdrEnv->prefilterShader->use();
	HdrEnv->prefilterShader->setInt("environmentMap", 9);
	HdrEnv->prefilterShader->setMat4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE9);

	glBindTexture(GL_TEXTURE_CUBE_MAP, HdrEnv->cubeTex->GetObjectID());

	

	unsigned int maxMipLevels = 5;
	for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
	{
		// reisze framebuffer according to mip-level size.
		unsigned int mipWidth = HdrEnv->prefilterMapTexSize * std::pow(0.5, mip);
		unsigned int mipHeight = HdrEnv->prefilterMapTexSize * std::pow(0.5, mip);

		
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		HdrEnv->prefilterShader->setFloat("roughness", roughness);
		for (unsigned int i = 0; i < 6; ++i)
		{
			HdrEnv->prefilterShader->setMat4("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, HdrEnv->prefilterMap->GetObjectID(), mip);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			DrawHDRCube(HdrEnv->prefilterShader, HdrEnv->HdrTexture);
		}
	}

	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void GameTechRenderer::ClearHDRBuffers()
{
	HdrEnv->clearHDRBuffers();
}


void NCL::CSC8503::GameTechRenderer::DrawHDRCube(OGLShader* shader, OGLTexture* tex)
{


	Transform* cubetransform = new Transform();

	BindShader(shader);
	//Transform* parentTransform, OGLMesh* mesh, TextureBase* colourtex, ShaderBase* shader
	RenderObject* i = new RenderObject(cubetransform, HdrEnv->cubeModel->meshes[0], tex, shader);
	BindMesh((*i).GetMesh());
	DrawBoundMesh();

}
