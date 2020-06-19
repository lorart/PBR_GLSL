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

			void InitWorld();

		
		
			
			/*
			These are some of the world/object creation functions I created when testing the functionality
			in the module. Feel free to mess around with them to see different objects being created in different
			test scenarios (constraints, collision types, and so on). 
			*/
			

			bool SelectObject();
			void MoveSelectedObject();
			//void DebugObjectMovement();
			void LockedObjectMovement();
			void LockedCameraMovement();

			void LightMovement();

			OGLLight* AddLightToWorld(Vector4 lightColour, float lightRadius, Vector3 lightPosition);
			void AddModelToWorld(Model* model, const Vector3& position, Vector3 dimensions, bool ispbr);
			void testShaderBySpheres();
			void AddHdrToWorld(OGLHdr* hdrEnvmap);


			


			GameTechRenderer*	renderer;
			PhysicsSystem*		physics;
			GameWorld*			world;

			bool useGravity;
			bool inSelectionMode;

			float		forceMagnitude;

			GameObject* selectionObject = nullptr;
			OGLTexture* basicTex = nullptr;
			OGLShader* basicShader = nullptr;
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

		};
	}
}

