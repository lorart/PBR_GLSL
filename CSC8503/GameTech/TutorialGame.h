#pragma once
#include "GameTechRenderer.h"
#include "../CSC8503Common/PhysicsSystem.h"
#include "../../Plugins/OpenGLRendering/Model.h"
#include "../../Plugins/OpenGLRendering/OGLHdr.h"



namespace NCL {
	namespace CSC8503 {
		class TutorialGame		{
		public:
			TutorialGame();
			~TutorialGame();

			virtual void UpdateGame(float dt);

		protected:
			void InitialiseAssets();

			void InitCamera();
			void UpdateKeys();
			void DrawDebugInformation();
			void InitWorld();

		
		
			
			/*
			These are some of the world/object creation functions I created when testing the functionality
			in the module. Feel free to mess around with them to see different objects being created in different
			test scenarios (constraints, collision types, and so on). 
			*/
			

			//bool SelectObject();
			void MoveSelectedObject();
			//void DebugObjectMovement();
			void LockedObjectMovement();
			void LockedCameraMovement();
			void ChangeShader();
			void ChangeModels();
			void ChangePossProcess();
			bool isShowSphereTest;
			bool isShowCamreaPos;
			

			vector<GameObject*> senceModel;
			vector<GameObject*> senceSphere;
			vector < vector<GameObject*> > senceSphereArry;


			void LightMovement();

			OGLLight* AddLightToWorld(Vector4 lightColour, float lightRadius, Vector3 lightPosition);
			vector<GameObject*> AddModelToWorld(Model* model, const Vector3& position, Vector3 dimensions, bool ispbr);
			void testShaderBySpheres();
			


			


			GameTechRenderer*	renderer;
			PhysicsSystem*		physics;
			GameWorld*			world;

			bool useGravity;
			bool inSelectionMode;
			bool IsUsePBRshader;
	

			float		forceMagnitude;

			GameObject* selectionObject = nullptr;
			OGLTexture* basicTex = nullptr;
			OGLShader* basicShader = nullptr;
			OGLShader* FongShader = nullptr;
			OGLHdr* hdrEnvmap= nullptr;
			

			
			Model* testmodel = nullptr;
			Model* sphere = nullptr;
			vector<Model*> testShaderModelVector;

			std::vector<OGLTexture*> modelPbrTex;


			GameObject* lockedObject	= nullptr;
			Vector3 lockedOffset		= Vector3(0, 14, 20);
			void LockCameraToObject(GameObject* o) {
				lockedObject = o;
			}

			string basicFragName;
			string basicVertName;

			string CompareFragName;
			string CompareVertName;

		};
	}
}

