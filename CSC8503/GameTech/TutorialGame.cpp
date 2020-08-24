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
	rabbitmodel = new Model(Assets::MESHDIR + "bunny" + ".obj", 0);



	basicTex = (OGLTexture*)TextureLoader::LoadAPITexture("checkerboard.png");
	//basicShader = new OGLShader("PBR_Vert.glsl", "PBR_Frag.glsl");
	basicVertName = "PBR_Vert.glsl";
	//basicFragName = "PBR_Frag_test.glsl";
	basicFragName = "PBR_Frag_ClearCoat.glsl";
	basicShader = new OGLShader(basicVertName, basicFragName);

	CompareVertName = "Feng_Vert.glsl";
	CompareFragName = "Feng_Frag.glsl";
	FongShader = new OGLShader("Feng_Vert.glsl", "Feng_Frag.glsl");

	string hdrFilename = "fireplace_1k.hdr";
	hdrEnvmap = new OGLHdr(hdrFilename);
	renderer->setupHDR(hdrEnvmap);
	isShowSphereTest = true;


	//initDebugStringArry();

	InitCamera();
	InitWorld();

}


TutorialGame::~TutorialGame()	{

	
	delete testmodel;
	delete rabbitmodel;
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
	//frameRate=CaculateFrameRate(dt);
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
	ChangeCameraType();
	ChangeMsaa();
	ChangeCameraFOVs();
	ChangeDOF();
	ChangeFocusDistance();

	//show debug;
	//ShowDebugDOV();
	
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
	
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::B)) {
		renderer->isUsedPBR = !renderer->isUsedPBR;
		//model->meshes[i]->material->pbrTexArry[ALBEDO_MAP],
	}
}

void TutorialGame::DrawDebugInformation()
{
     int leftUp=0;
	 int leftDown= 0;
	 int rightUp=0;
	 int rightDown=0;
	int debugScale=17;

	string temp;
	string temp_2;

	/*left down */
	Vector2 LeftDownP = Vector2(20,20);
	drawDebugString("MOVE Light: I J K L ",				LeftDownP + Vector2(0, debugScale * leftDown),1);				leftDown++;
	drawDebugString("MOVE Camera: S D Shift Space ",	LeftDownP + Vector2(0, debugScale * leftDown), 1);				leftDown++;
	drawDebugString("Camera Lens: Y+ H-",				LeftDownP + Vector2(0, debugScale * leftDown), 1);				leftDown++;
	drawDebugString("Focus distance: T+ G-",			LeftDownP + Vector2(0, debugScale * leftDown), 1);				leftDown++;
	drawDebugString("Change camera: R",					LeftDownP + Vector2(0, debugScale * leftDown), 1);				leftDown++;
	drawDebugString("Change Shader: B",					LeftDownP + Vector2(0, debugScale * leftDown), 1);				leftDown++;
	drawDebugString("Change Mesh: F",					LeftDownP + Vector2(0, debugScale * leftDown), 1);				leftDown++;
	drawDebugString("Anti-Aliasing: V",					LeftDownP + Vector2(0, debugScale * leftDown), 1);				leftDown++;
	drawDebugString("DEPTH of View : U",				LeftDownP + Vector2(0, debugScale * leftDown), 1);				leftDown++;

	/*left up*/
	Vector2 LeftUpP = Vector2(20, 630);
	drawDebugString_Switch("PBR shader", "Bilin-pohng shader", renderer->isUsedPBR, 
							LeftUpP - Vector2(0, debugScale * leftUp)); leftUp++;

	drawDebugString_Switch("Anti-Aliasing(MSAA) ON", "Anti-Aliasing OFF", renderer->isUsedMSAA,
							LeftUpP - Vector2(0, debugScale * leftUp)); leftUp++;


	drawDebugString_Switch("Physic Camera:", "Normal Camera", renderer->isUsedCamPos,
							LeftUpP - Vector2(0, debugScale * leftUp)); leftUp++;


	drawDebugString("",
		LeftUpP - Vector2(0, debugScale * leftUp), renderer->isUsedCamPos); leftUp++;

	drawDebugString("Full Frame Camrera",
		LeftUpP - Vector2(0, debugScale * leftUp), renderer->isUsedCamPos); leftUp++;

	drawDebugString_Switch("Depth of View ON", "Depth of View OFF", renderer->isUsedDov,
		LeftUpP - Vector2(0, debugScale * leftUp)); leftUp++;
	

	temp = std::to_string(world->GetMainCamera()->getLens());
	drawDebugString("Camera Lens  " + temp+" mm",
		LeftUpP - Vector2(0, debugScale * leftUp), renderer->isUsedCamPos); leftUp++;

	temp = std::to_string(world->GetMainCamera()->GetFieldOfVision());
	drawDebugString("Field of View  " + temp,
		LeftUpP - Vector2(0, debugScale * leftUp), renderer->isUsedCamPos); leftUp++;

	

	temp = std::to_string(world->GetMainCamera()->focusDistance);
	drawDebugString("Focus distance   " + temp, 
		LeftUpP - Vector2(0, debugScale * leftUp), renderer->isUsedCamPos); leftUp++;


	drawDebugString("lens projection:" ,
		LeftUpP - Vector2(0, debugScale * leftUp), renderer->isUsedCamPos); leftUp++;

	temp = std::to_string(world->GetMainCamera()->alphaX);
	temp_2 = std::to_string(world->GetMainCamera()->alphaY);
	drawDebugString("alphaX " + temp +"   alphaY "+ temp_2,
		LeftUpP - Vector2(0, debugScale * leftUp), renderer->isUsedCamPos); leftUp++;

	drawDebugString("",
		LeftUpP - Vector2(0, debugScale * leftUp), renderer->isUsedCamPos); leftUp++;

	
	Vector2 rightUpP = Vector2(650, 650);

	/*temp = std::to_string(frameRate);
	drawDebugString("Frame rate" + temp ,
		rightUpP - Vector2(0, debugScale * rightUp), renderer->isUsedCamPos); rightUp++;*/

	
	
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
		rabbitmodel->meshes[0]->material->albedoValue = Vector3(0.2, 0.2, 0.2);
		rabbitmodel->meshes[0]->material->metallicValue = 0.9;
		rabbitmodel->meshes[0]->material->roughnessValue =0.1;
		auto temp = AddModelToWorld(rabbitmodel, Vector3(-40, 0, -40), Vector3(30, 30, 30), isPBR);
		senceModel.insert(senceModel.end(),temp.begin(), temp.end());

		rabbitmodel = new Model(Assets::MESHDIR + "bunny" + ".obj", 0);
		rabbitmodel->meshes[0]->material->albedoValue = Vector3(0.2, 0.2, 0.2);
		rabbitmodel->meshes[0]->material->metallicValue = 0.9;
		rabbitmodel->meshes[0]->material->roughnessValue = 0.1;
		rabbitmodel->meshes[0]->material->clearCoat=1;
		rabbitmodel->meshes[0]->material->isUseClearcoat = 1;
		rabbitmodel->meshes[0]->material->clearCoatPerceptualRoughness = 0.2;


		 temp = AddModelToWorld(rabbitmodel, Vector3(-60, 0, -20), Vector3(30, 30, 30), isPBR);
		senceModel.insert(senceModel.end(), temp.begin(), temp.end());

		for (auto& i :senceSphereArry)
		{
			for (auto& j : i) {
				world->RemoveGameObject(j);
			}

		}

	}
}
	
}

void TutorialGame::ChangeCameraType()
{

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::R)) {
		//std::cout << "***R pressed " << std::endl;
		renderer->isUsedCamPos = !renderer->isUsedCamPos;
		if (!renderer->isUsedCamPos)
		{
			renderer->isUsedDov = false;

		}
		else {
			renderer->isUsedDov = true;

		}
		
	}
		
}

void TutorialGame::ChangeCameraFOVs()
{
	float scale = 0.3;
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::Y)) {
		float temp = world->GetMainCamera()->getLens();
		
		if (temp<=50-scale) {
			
			world->GetMainCamera()->setLens(temp + scale);
			//std::cout << "lens=  " << world->GetMainCamera()->getLens() << std::endl;
			world->GetMainCamera()->SetFov(world->GetMainCamera()->caculateFieldofView());
			world->GetMainCamera()->alphaX=world->GetMainCamera()->caculateAlphaX();
			world->GetMainCamera()->alphaY=world->GetMainCamera()->caculateAlphaY();
		}
	}
	if (Window::GetKeyboard()-> KeyDown(KeyboardKeys::H)) {
		float temp = world->GetMainCamera()-> getLens();
		if (temp >= 35+ scale) {
			world->GetMainCamera()->setLens(temp - scale);
			world->GetMainCamera()->SetFov(world->GetMainCamera()->caculateFieldofView());
			world->GetMainCamera()->alphaX = world->GetMainCamera()->caculateAlphaX();
			world->GetMainCamera()->alphaY = world->GetMainCamera()->caculateAlphaY();
	
		}
	}

	 
}


void TutorialGame::drawDebugString(string debugString,Vector2 Spos ,bool condition)
{
	if (condition) {
		Debug::Print(debugString, Spos, Vector4(1, 1, 1, 1));
	}

}
void TutorialGame::drawDebugString_Switch(string debugString1, string debugString2, bool IsString1,Vector2 Spos)
{
	string usedString;
	if (IsString1) {
		usedString = debugString1;
	}
	else {
		usedString = debugString2;
	}

		Debug::Print(usedString, Spos);

	
}



void NCL::CSC8503::TutorialGame::ChangeMsaa()
{
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::V)) {
		//std::cout << "***R pressed " << std::endl;
		renderer->isUsedMSAA = !renderer->isUsedMSAA;
		
	}


}

void NCL::CSC8503::TutorialGame::ChangeFocusDistance()
{
	float scale = 0.1;
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::T)) {
		world->GetMainCamera()->focusDistance= world->GetMainCamera()->focusDistance+ scale;
	
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::G)) {
		if (world->GetMainCamera()->focusDistance>= scale) {
			world->GetMainCamera()->focusDistance = world->GetMainCamera()->focusDistance - scale;

		}


	}



}

int TutorialGame::CaculateFrameRate(float deltaTime)
{
	static float temp = 0;
	static float count = 0;
	static int oldtime = 0;
if (count<30)
{
	temp += deltaTime;
	count++;
}
else {
	oldtime = static_cast<int>(1.f / (deltaTime/30));
	count = 0;
	return oldtime;
}
	
return oldtime;
}

void TutorialGame::ShowDebugDOV()
{
	float lens = world->GetMainCamera()->getLens();
	float  lens_m = world->GetMainCamera()->getLens()*0.001;
	float cameraFocusDistance = world->GetMainCamera()->focusDistance;
	float Coc = 0.029;//Circle of confusion 35mm  0.029
	float N = 1.8;
	float lens_aperture = lens / N;

	float CFD = cameraFocusDistance  + 0.001;

	
	float H = (lens * lens / (N * Coc)+ lens)*0.001;
	//float H = 6.25;


	float near_Fplane = (H * CFD) / (H + CFD - lens_m);

	float far_Fplane = (H * CFD) / (H -CFD - lens_m);


	float near_distance = 0.7 * (0 - near_Fplane); // 
	float far_distance = 0.7 * (0 - far_Fplane); //
	

	drawDebugString("DebugDOV:",
		 Vector2(600,620), renderer->isUsedCamPos); 

	drawDebugString("H:" + std::to_string(H),
		Vector2(600, 600), renderer->isUsedCamPos);

	drawDebugString("near_Fplane:" + std::to_string(near_Fplane),
		Vector2(600, 580), renderer->isUsedCamPos); ;
	drawDebugString("far_Fplane:" + std::to_string(far_Fplane),
		Vector2(600, 560), renderer->isUsedCamPos); ;


	drawDebugString("near_distance:"+ std::to_string(near_distance),
		Vector2(600, 540), renderer->isUsedCamPos); ;
	drawDebugString("far_distance:" + std::to_string(far_distance),
		Vector2(600, 520), renderer->isUsedCamPos); ;


}

//
//
//void NCL::CSC8503::TutorialGame::y()
//{
//	float scale = 10;
//	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::T)) {
//		world->GetMainCamera()->focusDistance = world->GetMainCamera()->focusDistance + scale;
//
//	}
//	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::G)) {
//		world->GetMainCamera()->focusDistance = world->GetMainCamera()->focusDistance - scale;
//
//	}
//
//
//
//}

void TutorialGame::ChangeDOF()
{
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::U)&& renderer->isUsedPBR) {
		//std::cout << "***R pressed " << std::endl;
		renderer->isUsedDov = !renderer->isUsedDov;

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
		//todo:delete light
		//std::cout << renderer->lightArry[0]->lightPosition << std::endl;
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
	world->GetMainCamera()->SetFarPlane(1000.0f);
	world->GetMainCamera()->SetPitch(-15.0f);
	world->GetMainCamera()->SetYaw(315.0f);
	world->GetMainCamera()->SetPosition(Vector3(-60, 40, 60));
	world->GetMainCamera()->focusDistance=1;
	lockedObject = nullptr;
}

void TutorialGame::InitWorld() {
	world->ClearAndErase();
	
	bool isPBR=true;

	 testShaderBySpheres();
	AddLightToWorld(Vector4(1, 1, 1, 1), 500, Vector3(50, 200, 50));


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
	//for (int depth = 0; depth < 2; depth++) {
	int hightNum = 4;
	int wideNum = 5;
		for (int hight = 0; hight <= hightNum; hight++)
		{
			tempValue = 0;


			for (int wide = 0; wide <wideNum; wide++)
			{
				sphere = new Model(Assets::MESHDIR + "sphere" + ".obj", 0);
				testShaderModelVector.push_back(sphere);
				tempValue = (wide) * (1.0 / wideNum);
				senceSphere = AddModelToWorld(sphere, Vector3(-hight * length, wide * length, 1), Vector3(5, 5, 5), IsUsePBRshader);

				sphere->meshes[0]->material->metallicValue = hight * (1.0 / wideNum);

				sphere->meshes[0]->material->roughnessValue = tempValue;
				//std::cout << "  hight=" << hight << "  wide=" << wide << "  tempValue=" << tempValue << "  wide* 1 /5=" << wide * 1 / 5 << std::endl;

				sphere->meshes[0]->material->albedoValue = clolor;
				senceSphereArry.push_back(senceSphere);


			}
		//}
	}
	
		//clear coat
		for (int wide = 0; wide < wideNum; wide++)
		{
			sphere = new Model(Assets::MESHDIR + "sphere" + ".obj", 0);
			testShaderModelVector.push_back(sphere);

			senceSphere = AddModelToWorld(sphere, Vector3(-(hightNum+2) * length, wide * length, 1), Vector3(5, 5, 5), IsUsePBRshader);

			sphere->meshes[0]->material->metallicValue = 0.1;
			sphere->meshes[0]->material->roughnessValue = 0.2;

			sphere->meshes[0]->material->isUseClearcoat =1;
			sphere->meshes[0]->material->clearCoatPerceptualRoughness = wide * (1.0 / wideNum);
			//std::cout << "----clearCoatPerceptualRoughness  " << wide * (1.0 / wideNum) << std::endl;

			sphere->meshes[0]->material->clearCoat =1;
			//sphere->meshes[0]->material->roughnessValue = tempValue;
			//std::cout << "  hight=" << hight << "  wide=" << wide << "  tempValue=" << tempValue << "  wide* 1 /5=" << wide * 1 / 5 << std::endl;

			sphere->meshes[0]->material->albedoValue = clolor;
			senceSphereArry.push_back(senceSphere);


		}

}



