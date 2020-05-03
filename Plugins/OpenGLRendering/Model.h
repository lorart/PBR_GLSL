#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "../../Common/MeshGeometry.h"
#include "glad\glad.h"
#include "Mesh.h"
#include <string>

/*

References:
https://learnopengl.com/Model-Loading/Model

*/

namespace NCL {
	namespace Rendering {
		class Model
		{

		public:
			vector<Mesh>   meshes;
			std::string directory;
			bool gammaCorrection;

			// constructor, expects a filepath to a 3D model.
			Model(std::string const& path, bool gamma = false) : gammaCorrection(gamma)
			{
				loadModel(path);
			}

		private:
			// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
			void loadModel(std::string const& path)
			{
				// read file via ASSIMP
				Assimp::Importer importer;
				const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
				// check for errors
				if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
				{
					std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
					return;
				}
				// retrieve the directory path of the filepath
				directory = path.substr(0, path.find_last_of('/'));

				// process ASSIMP's root node recursively
				processNode(scene->mRootNode, scene);
			}

			// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
			void processNode(aiNode* node, const aiScene* scene)
			{
				// process each mesh located at the current node
				for (unsigned int i = 0; i < node->mNumMeshes; i++)
				{
					// the node object only contains indices to index the actual objects in the scene. 
					// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
					aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];


					meshes.push_back(processMesh(mesh, scene));


				}
				// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
				for (unsigned int i = 0; i < node->mNumChildren; i++)
				{
					processNode(node->mChildren[i], scene);
				}

			}

			Mesh processMesh(aiMesh* mesh, const aiScene* scene)
			{
				//TODO
				std::cout << "Process Mesh" << std::endl;




			}
		};
	}
};

