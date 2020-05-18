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

GameTechRenderer::GameTechRenderer(GameWorld& world) : OGLRenderer(*Window::GetWindow()), gameWorld(world)	{
	glEnable(GL_DEPTH_TEST);

	shadowShader = new OGLShader("GameTechShadowVert.glsl", "GameTechShadowFrag.glsl");

	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
			     SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(1, 1, 1, 1);

	//Set up the light properties
	lightColour = Vector4(0.8f, 0.8f, 0.5f, 1.0f);
	lightRadius = 1000.0f;
	lightPosition = Vector3(-200.0f, 60.0f, -200.0f);
}

GameTechRenderer::~GameTechRenderer()	{
	glDeleteTextures(1, &shadowTex);
	glDeleteFramebuffers(1, &shadowFBO);
}

void GameTechRenderer::RenderFrame() {
	glEnable(GL_CULL_FACE);
	glClearColor(1, 1, 1, 1);
	BuildObjectList();
	SortObjectList();
	RenderShadowMap();
	RenderCamera();
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
			const RenderObject*g = (*i)->GetRenderObject();
			if (g) {
				activeObjects.emplace_back(g);
			}
		}
	}
}

void GameTechRenderer::SortObjectList() {

}

void GameTechRenderer::RenderShadowMap() {
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);

	glCullFace(GL_FRONT);

	BindShader(shadowShader);
	int mvpLocation = glGetUniformLocation(shadowShader->GetProgramID(), "mvpMatrix");

	Matrix4 shadowViewMatrix = Matrix4::BuildViewMatrix(lightPosition, Vector3(0, 0, 0), Vector3(0,1,0));
	Matrix4 shadowProjMatrix = Matrix4::Perspective(100.0f, 500.0f, 1, 45.0f);

	Matrix4 mvMatrix = shadowProjMatrix * shadowViewMatrix;

	shadowMatrix = biasMatrix * mvMatrix; //we'll use this one later on

	for (const auto&i : activeObjects) {
		Matrix4 modelMatrix = (*i).GetTransform()->GetWorldMatrix();
		Matrix4 mvpMatrix	= mvMatrix * modelMatrix;
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
	float screenAspect = (float)currentWidth / (float)currentHeight;
	Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);

	OGLShader* activeShader = nullptr;
	int projLocation	= 0;
	int viewLocation	= 0;
	int modelLocation	= 0;
	//int colourLocation  = 0;
	//int hasVColLocation = 0;
	int hasTexLocation  = 0;
	int shadowLocation  = 0;

	int lightPosLocation	= 0;
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
		const auto i = activeObjects[l];



		OGLShader* shader = (OGLShader*)(*i).GetShader();
		BindShader(shader);
		//todo:why
		//if (activeShader != shader) {
			if ((*i).GetIsPBR()) {
				//TODO: 

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

				albedoValueLocation = glGetUniformLocation(shader->GetProgramID(), "albedoValue");
				normalValueLocation = glGetUniformLocation(shader->GetProgramID(), "normalValue");
				metallicValueLocation = glGetUniformLocation(shader->GetProgramID(), "metallicValue");
				roughnessValueLocation = glGetUniformLocation(shader->GetProgramID(), "roughnessValue");
				aoValueLocation = glGetUniformLocation(shader->GetProgramID(), "aoValue");

				glUniform3fv(albedoValueLocation, 1, (float*)&(tempMaterial->albedoValue));
				glUniform3fv(normalValueLocation, 1, (float*)&(tempMaterial->normalValue));
				glUniform1f(metallicValueLocation, tempMaterial->metallicValue);
				glUniform1f(roughnessValueLocation, tempMaterial->roughnessValue);
				glUniform1f(aoValueLocation, tempMaterial->aoValue);


			}
			else {
				BindTextureToShader((OGLTexture*)(*i).GetDefaultTexture(), "mainTex", 0);

				hasTexLocation = glGetUniformLocation(shader->GetProgramID(), "hasTexture");

			}



			projLocation = glGetUniformLocation(shader->GetProgramID(), "projMatrix");
			viewLocation = glGetUniformLocation(shader->GetProgramID(), "viewMatrix");
			modelLocation = glGetUniformLocation(shader->GetProgramID(), "modelMatrix");
			shadowLocation = glGetUniformLocation(shader->GetProgramID(), "shadowMatrix");
			//	colourLocation  = glGetUniformLocation(shader->GetProgramID(), "objectColour");
			//	hasVColLocation = glGetUniformLocation(shader->GetProgramID(), "hasVertexColours");
			lightPosLocation = glGetUniformLocation(shader->GetProgramID(), "lightPos");
			lightColourLocation = glGetUniformLocation(shader->GetProgramID(), "lightColour");
			lightRadiusLocation = glGetUniformLocation(shader->GetProgramID(), "lightRadius");

			cameraLocation = glGetUniformLocation(shader->GetProgramID(), "cameraPos");
			glUniform3fv(cameraLocation, 1, (float*)&gameWorld.GetMainCamera()->GetPosition());

			glUniformMatrix4fv(projLocation, 1, false, (float*)&projMatrix);
			glUniformMatrix4fv(viewLocation, 1, false, (float*)&viewMatrix);

			glUniform3fv(lightPosLocation, 1, (float*)&lightPosition);
			glUniform4fv(lightColourLocation, 1, (float*)&lightColour);
			glUniform1f(lightRadiusLocation, lightRadius);

			int shadowTexLocation = glGetUniformLocation(shader->GetProgramID(), "shadowTex");
			glUniform1i(shadowTexLocation, 1);
			activeShader = shader;
		//}

		//textureRelate
		//?
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
		glActiveTexture(GL_TEXTURE0);
	}
	//}
}
	

void GameTechRenderer::SetupDebugMatrix(OGLShader*s) {
	float screenAspect = (float)currentWidth / (float)currentHeight;
	Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);

	Matrix4 vp = projMatrix * viewMatrix;

	int matLocation = glGetUniformLocation(s->GetProgramID(), "viewProjMatrix");

	glUniformMatrix4fv(matLocation, 1, false, (float*)&vp);
}