#include "TutorialGame.h"
#include "../CSC8503Common/GameWorld.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Common/TextureLoader.h"

#include "../CSC8503Common/PositionConstraint.h"
#include "../../Plugins/OpenGLRendering/Model.h"
#include "../../Common/Assets.h"

#include "../../Plugins/OpenGLRendering/OGLHdr.h"

using namespace NCL;
using namespace CSC8503;

TutorialGame::TutorialGame()	{
	world		= new GameWorld();
	renderer	= new GameTechRenderer(*world);
#pragma region
	/*physics		= new PhysicsSystem(*world);

	forceMagnitude	= 10.0f;
	useGravity		= false;
	inSelectionMode = false;*/
#pragma endregion physic

	IsUsePBRshader = true;
	Debug::SetRenderer(renderer);

	InitialiseAssets();
}

/*

Each of the little demo scenarios used in the game uses the same 2 meshes, 
and the same texture and shader. There's no need to ever load in anything else
for this module, even in the coursework, but you can add it if you like!

*/
void TutorialGame::InitialiseAssets() {

	auto loadFunc = [](const string& name, OGLMesh** into) {
		*into = new OGLMesh(name);
		(*into)->SetPrimitiveType(GeometryPrimitive::Triangles);
		(*into)->UploadToGPU();
	};

	//TODO:DELETE
	//loadFunc("cube.msh"	 , &cubeMesh);
	string modelname = "apple";
	testmodel = new Model(Assets::MESHDIR + modelname + ".obj", 0);

	basicTex = (OGLTexture*)TextureLoader::LoadAPITexture("checkerboard.png");
	//basicShader = new OGLShader("PBR_Vert.glsl", "PBR_Frag.glsl");
	basicVertName = "PBR_Vert.glsl";
	basicFragName = "PBR_Frag.glsl";
	basicShader = new OGLShader(basicVertName, basicFragName);

	CompareVertName = "Feng_Vert.glsl";
	CompareFragName = "Feng_Frag.glsl";
	FongShader = new OGLShader("Feng_Vert.glsl", "Feng_Frag.glsl");

	string hdrFilename = "fireplace_1k.hdr";
	hdrEnvmap = new OGLHdr(hdrFilename);
	renderer->setupHDR(hdrEnvmap);
	isShowSphereTest = true;

	InitCamera();
	InitWorld();
}

TutorialGame::~TutorialGame()	{

	
	delete testmodel;
	for (auto& i:testShaderModelVector)
	{
		delete i;
	}
	delete sphere;
	delete basicTex;
	delete basicShader;
	delete FongShader;
	delete hdrEnvmap;
	
#pragma region
	//delete physics;
#pragma endregion physic
	delete renderer;
	delete world;
}

void TutorialGame::UpdateGame(float dt) {
	if (!inSelectionMode) {
		world->GetMainCamera()->UpdateCamera(dt);
	}
	if (lockedObject != nullptr) {
		LockedCameraMovement();
	}

	UpdateKeys();
#pragma region
	//if (useGravity) {
	//	Debug::Print("(G)ravity on", Vector2(10, 40));
	//}
	//else {
	//	Debug::Print("(G)ravity off", Vector2(10, 40));
	//}
#pragma endregion physic
	//SelectObject();
	MoveSelectedObject();

	world->UpdateWorld(dt);
	renderer->Update(dt);
#pragma region
	//physics->Update(dt);
#pragma endregion physic
	Debug::FlushRenderables();
	DrawDebugInformation();
	renderer->Render();
}

void TutorialGame::UpdateKeys() {
	

	if (lockedObject) {
		LockedObjectMovement();
	}
	else {
		//DebugObjectMovement();
	}
	LightMovement();
	ChangeShader();
	ChangeModels();
	ChangePossProcess();
	ChangeMsaa();
	
}

void TutorialGame::LockedObjectMovement() {
	Matrix4 view		= world->GetMainCamera()->BuildViewMatrix();
	Matrix4 camWorld	= view.Inverse();

	Vector3 rightAxis = Vector3(camWorld.GetColumn(0)); //view is inverse of model!

	//forward is more tricky -  camera forward is 'into' the screen...
	//so we can take a guess, and use the cross of straight up, and
	//the right axis, to hopefully get a vector that's good enough!

	Vector3 fwdAxis = Vector3::Cross(Vector3(0, 1, 0), rightAxis);

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
		selectionObject->GetPhysicsObject()->AddForce(-rightAxis);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
		selectionObject->GetPhysicsObject()->AddForce(rightAxis);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
		selectionObject->GetPhysicsObject()->AddForce(fwdAxis);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
		selectionObject->GetPhysicsObject()->AddForce(-fwdAxis);
	}
}

void  TutorialGame::LockedCameraMovement() {
	if (lockedObject != nullptr) {
		Vector3 objPos = lockedObject->GetTransform().GetWorldPosition();
		Vector3 camPos = objPos + lockedOffset;

		Matrix4 temp = Matrix4::BuildViewMatrix(camPos, objPos, Vector3(0, 1, 0));

		Matrix4 modelMat = temp.Inverse();

		Quaternion q(modelMat);
		Vector3 angles = q.ToEuler(); //nearly there now!

		world->GetMainCamera()->SetPosition(camPos);
		world->GetMainCamera()->SetPitch(angles.x);
		world->GetMainCamera()->SetYaw(angles.y);
	}
}

void TutorialGame::ChangeShader()
{
	
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::G)) {
		renderer->isUsedPBR = !renderer->isUsedPBR;
		//model->meshes[i]->material->pbrTexArry[ALBEDO_MAP],
	}
}

void TutorialGame::DrawDebugInformation()
{
	renderer->DrawString("Anti-Aliasing on/off: T", Vector2(20, 100));
	renderer->DrawString("Change camera: R", Vector2(20, 80));
	renderer->DrawString("Change Shader: G", Vector2(20, 20));
	renderer->DrawString("Change Mesh: F", Vector2(20, 60));
	renderer->DrawString("MOVE Camera: S D Shift Space ", Vector2(20, 0));
	renderer->DrawString("MOVE Light: I J K L ", Vector2(20, 40));

	if (renderer->isUsedPBR) {
		renderer->DrawString("PBR shader", Vector2(20, 600));
	}
	else {
		renderer->DrawString("Bilin-pohng shader", Vector2(20, 600));
	}
	if (renderer->isUsedCamPos) {
		renderer->DrawString("Physic Camera", Vector2(20, 620));
	}
	else {
		renderer->DrawString("Normal Camerar", Vector2(20, 620));
	}
	if (renderer->isUsedMSAA) {
		renderer->DrawString("Anti-Aliasing(MSAA) ON", Vector2(20, 640));
	}
	else {
		renderer->DrawString("Anti-Aliasing OFF", Vector2(20, 640));
	}



}

void TutorialGame::ChangeModels()
{
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F)) {
	//	std::cout << "***E pressed " << std::endl;
		isShowSphereTest = !isShowSphereTest;
		if (isShowSphereTest) {

			//std::cout << "***testShaderBySpheres" << std::endl;
			testShaderBySpheres();
			for (auto& i : senceModel) {
				world->RemoveGameObject(i);
			}

		}
	else {
	//	std::cout << "***AddModelToWorld" << std::endl;
		bool isPBR = true;
		senceModel=	AddModelToWorld(testmodel, Vector3(0, 0, 0), Vector3(2, 2, 2), isPBR);
		for (auto& i :senceSphereArry)
		{
			for (auto& j : i) {
				world->RemoveGameObject(j);
			}

		}

	}
}
	
}

void TutorialGame::ChangePossProcess()
{

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::R)) {
		//std::cout << "***R pressed " << std::endl;
		renderer->isUsedCamPos = !renderer->isUsedCamPos;
	}
		
}

void NCL::CSC8503::TutorialGame::ChangeMsaa()
{
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::T)) {
		//std::cout << "***R pressed " << std::endl;
		renderer->isUsedMSAA = !renderer->isUsedMSAA;
	}

}

void NCL::CSC8503::TutorialGame::LightMovement()
{//todo:only move one light
	if (renderer->lightArry.size() > 0) {
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::J)) {
			renderer->lightArry[0]->lightPosition = renderer->lightArry[0]->lightPosition+Vector3(0,0,-1);
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::K)) {
			renderer->lightArry[0]->lightPosition = renderer->lightArry[0]->lightPosition + Vector3(1, 0,0);
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::L)) {
			renderer->lightArry[0]->lightPosition = renderer->lightArry[0]->lightPosition + Vector3(0, 0, 1);
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::I)) {
			renderer->lightArry[0]->lightPosition = renderer->lightArry[0]->lightPosition + Vector3(-1, 0, 0);
		}
	}
}




/*
If an object has been clicked, it can be pushed with the right mouse button, by an amount
determined by the scroll wheel. In the first tutorial this won't do anything, as we haven't
added linear motion into our physics system. After the second tutorial, objects will move in a straight
line - after the third, they'll be able to twist under torque aswell.
*/

void TutorialGame::MoveSelectedObject() {

}

void TutorialGame::InitCamera() {
	world->GetMainCamera()->SetNearPlane(30.0f);
	world->GetMainCamera()->SetFarPlane(500.0f);
	world->GetMainCamera()->SetPitch(-15.0f);
	world->GetMainCamera()->SetYaw(315.0f);
	world->GetMainCamera()->SetPosition(Vector3(-60, 40, 60));
	lockedObject = nullptr;
}

void TutorialGame::InitWorld() {
	world->ClearAndErase();
	
	bool isPBR=true;

	 testShaderBySpheres();
	AddLightToWorld(Vector4(1, 1, 1, 1), 100, Vector3(10, 80, 10));


	//physics->Clear();

}

//From here on it's functions to add in objects to the world!


vector<GameObject*> TutorialGame::AddModelToWorld(Model* model,const Vector3& position, Vector3 dimensions, bool ispbr) {
	vector<GameObject*> tempGameObjectArry;
	for (size_t i = 0; i < model->meshes.size(); i++)
	{
		
		GameObject* modelObject = new GameObject();
		AABBVolume* volume = new AABBVolume(dimensions);
		modelObject->SetBoundingVolume((CollisionVolume*)volume);

		modelObject->GetTransform().SetWorldPosition(position);
		modelObject->GetTransform().SetWorldScale(dimensions);

		model->meshes[i]->material->matShader = new OGLShader(basicVertName, basicFragName);
		model->meshes[i]->material->FengShader = new OGLShader(CompareVertName,CompareFragName);

		if (ispbr)
		{
			
		//	model->meshes[i]->material->matShader = new OGLShader(basicVertName, basicFragName);
			modelObject->SetRenderObject(new RenderObject(&modelObject->GetTransform(), model->meshes[i], model->meshes[i]->material, 
				model->meshes[i]->material->matShader, model->meshes[i]->material->FengShader, true));
		} 
		else {
		/*
			modelObject->SetRenderObject(new RenderObject(&modelObject->GetTransform(), model->meshes[i], 
				model->meshes[i]->material->pbrTexArry[ALBEDO_MAP], basicShader));*/
			modelObject->SetRenderObject(new RenderObject(&modelObject->GetTransform(), model->meshes[i],
				model->meshes[i]->material->pbrTexArry[ALBEDO_MAP], model->meshes[i]->material->FengShader));
		}
	
		modelObject->SetPhysicsObject(new PhysicsObject(&modelObject->GetTransform(), modelObject->GetBoundingVolume()));

		modelObject->GetPhysicsObject()->SetInverseMass(0);
		modelObject->GetPhysicsObject()->InitCubeInertia();

		world->AddGameObject(modelObject);
		tempGameObjectArry.push_back(modelObject);
	}
	return tempGameObjectArry;

}



OGLLight* NCL::CSC8503::TutorialGame::AddLightToWorld(Vector4 lightColour, float lightRadius, Vector3 lightPosition)
{
	OGLLight* light = new OGLLight(lightColour, lightRadius, lightPosition);
	renderer->lightArry.push_back(light);
	return light;
}

void NCL::CSC8503::TutorialGame::testShaderBySpheres()
{
	float tempValue = 0;
	int length=15;
	Vector3 clolor = Vector3(0, 0, 1);
	for (int hight = 0; hight < 4; hight++)
	{
		 tempValue = 0;


		for (int wide = 0; wide <= 5; wide++)
		{
			sphere = new Model(Assets::MESHDIR + "sphere" + ".obj", 0);
			testShaderModelVector.push_back(sphere);
			tempValue =wide*(1.0 /5.0);
			senceSphere=	AddModelToWorld(sphere, Vector3(-hight * length, wide * length, 0), Vector3(5, 5, 5), IsUsePBRshader);
			
				sphere->meshes[0]->material->metallicValue = hight * (1.0 / 5.0);
				
				sphere->meshes[0]->material->roughnessValue= tempValue;
				//std::cout << "  hight=" << hight << "  wide=" << wide << "  tempValue=" << tempValue << "  wide* 1 /5=" << wide * 1 / 5 << std::endl;
			
				sphere->meshes[0]->material->albedoValue = clolor;
				senceSphereArry.push_back(senceSphere);
			
			
		}
	}
	

}



