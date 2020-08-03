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

	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(1, 1, 1, 1);
#pragma endregion
	isUsedPBR = true;
	HdrEnv = nullptr;
	float screenAspect = 0;
	Matrix4 viewMatrix;
	Matrix4 projMatrix;
	CompareShader = nullptr;


}

GameTechRenderer::~GameTechRenderer() {
	glDeleteTextures(1, &shadowTex);
	glDeleteFramebuffers(1, &shadowFBO);
	//todo:check
	delete HdrEnv;
	delete CompareShader;

	for (auto& i : lightArry) {
		delete i;
	}

}

void GameTechRenderer::RenderFrame() {
	CaculateViewPorjMat();
	glEnable(GL_CULL_FACE);
	glClearColor(1, 1, 1, 1);
	
	//todo:delete
	setupHDR(HdrEnv);

	BuildObjectList();
	SortObjectList();
	RenderShadowMap();
	RenderCamera();
	RenderHDRSkybox();
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
//todo:shadowMap
//void GameTechRenderer::RenderShadowMap() {
//
//	glEnable(GL_DEPTH_TEST);
//	glDepthFunc(GL_LESS);
//
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
//	glClear(GL_DEPTH_BUFFER_BIT);//	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);//	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);
//
//	glCullFace(GL_FRONT);
//
//	
//	BindShader(shadowShader);
//	int mvpLocation = glGetUniformLocation(shadowShader->GetProgramID(), "mvpMatrix");
//
//	Matrix4 shadowViewMatrix = Matrix4::BuildViewMatrix(lightArry[0]->lightPosition, Vector3(0, 0, 0), Vector3(0,1,0));
//
//	Matrix4 shadowProjMatrix = Matrix4::Perspective(100.0f, 500.0f, 1, 45.0f);
//
//	Matrix4 mvMatrix = shadowProjMatrix * shadowViewMatrix;
//
//	shadowMatrix = biasMatrix * mvMatrix; //we'll use this one later on
//
//	for (const auto&i : activeObjects) {
//		Matrix4 modelMatrix = (*i).GetTransform()->GetWorldMatrix();
//		Matrix4 mvpMatrix	= mvMatrix * modelMatrix;
//		glUniformMatrix4fv(mvpLocation, 1, false, (float*)&mvpMatrix);
//		BindMesh((*i).GetMesh());
//		DrawBoundMesh();
//	}	
//
//	glViewport(0, 0, currentWidth, currentHeight);
//	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//	glCullFace(GL_BACK);
//}
void GameTechRenderer::RenderShadowMap() {

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);


	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);

	glCullFace(GL_FRONT);

	BindShader(shadowShader);
	int mvpLocation = glGetUniformLocation(shadowShader->GetProgramID(), "mvpMatrix");

	Matrix4 shadowViewMatrix = Matrix4::BuildViewMatrix(lightArry[0]->lightPosition, Vector3(0, 0, 0), Vector3(0, 1, 0));
	Matrix4 shadowProjMatrix = Matrix4::Perspective(100.0f, 500.0f, 1, 45.0f);

	Matrix4 mvMatrix = shadowProjMatrix * shadowViewMatrix;

	shadowMatrix = biasMatrix * mvMatrix; //we'll use this one later on

	for (const auto& i : activeObjects) {
		Matrix4 modelMatrix = (*i).GetTransform()->GetWorldMatrix();
		Matrix4 mvpMatrix = mvMatrix * modelMatrix;
		glUniformMatrix4fv(mvpLocation, 1, false, (float*)&mvpMatrix);
		BindMesh((*i).GetMesh());
		DrawBoundMesh();
	}

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
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	//for (const auto& i : activeObjects) {
	for (size_t l = 0; l < activeObjects.size(); l++)
	{
		OGLShader* shader;
		const auto i = activeObjects[l];
	
		 if (isUsedPBR) {
		 shader = (OGLShader*)(*i).GetShader();
			// shader = CompareShader;
		 }
		 else {
			 shader=CompareShader;
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
				}



				/*	glUniform3fv(albedoValueLocation, 1, (float*)&(tempMaterial->albedoValue));
					glUniform3fv(normalValueLocation, 1, (float*)&(tempMaterial->normalValue));
					glUniform1f(metallicValueLocation, tempMaterial->metallicValue);
					glUniform1f(roughnessValueLocation, tempMaterial->roughnessValue);
					glUniform1f(aoValueLocation, tempMaterial->aoValue);*/


			}
			else {
			
				BindTextureToShader((OGLTexture*)(*i).GetDefaultTexture(), "mainTex", 6);

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
			glUniform1i(shadowTexLocation, 1);
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

		Matrix4 fullShadowMat = shadowMatrix * modelMatrix;
		glUniformMatrix4fv(shadowLocation, 1, false, (float*)&fullShadowMat);

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
}

void NCL::CSC8503::GameTechRenderer::CaculateViewPorjMat()
{
	this->screenAspect = (float)currentWidth / (float)currentHeight;
	this->viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	this->projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);

}

void NCL::CSC8503::GameTechRenderer::RenderHDRSkybox()
{

	glDepthFunc(GL_LEQUAL);


	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_CUBE_MAP, HdrEnv->cubeTex->GetObjectID());

	HdrEnv->SkyboxShader->use();
	HdrEnv->SkyboxShader->setMat4("projection", this->projMatrix);
	HdrEnv->SkyboxShader->setMat4("view", this->viewMatrix);

	HdrEnv->SkyboxShader->setInt("environmentMap", 10);

	DrawHDRCube(HdrEnv->SkyboxShader, HdrEnv->cubeTex);



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

	//glBindFramebuffer(GL_FRAMEBUFFER, HdrEnv->captureFBO);
	generate_bind_Fbo(HdrEnv->captureFBO);

	for (int i = 0; i < 6; i++)
	{
		HdrEnv->HdrToCubemapShader->setMat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, HdrEnv->cubeTex->GetObjectID(), 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		DrawHDRCube(HdrEnv->HdrToCubemapShader, HdrEnv->HdrTexture);
		//renderHDR to  cubemap

	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &HdrEnv->captureFBO);//todo::test



}

void NCL::CSC8503::GameTechRenderer::RenderCubemaptoIrradianceMap()
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

	//todo:test
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, HdrEnv->cubeIrradianceTexSize,
		HdrEnv->cubeIrradianceTexSize);
	HdrEnv->irradianceShader->use();
	
	HdrEnv->irradianceShader->setInt("environmentMap", 0);
	HdrEnv->irradianceShader->setMat4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE11);
	glBindTexture(GL_TEXTURE_CUBE_MAP, HdrEnv->irradianceMap->GetObjectID());

	glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, HdrEnv->captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		HdrEnv->irradianceShader->setMat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, HdrEnv->irradianceMap->GetObjectID(), 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		DrawHDRCube(HdrEnv->irradianceShader, HdrEnv->HdrTexture);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
