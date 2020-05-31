#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "../../Common/MeshGeometry.h"
#include "glad\glad.h"
#include "OGLMesh.h"
#include <string>
#include"OGLTexture.h"
#include "..\..\Common\Assets.h"
#include "../../CSC8503/CSC8503Common/RenderObject.h"

#include "../../Common/TextureLoader.h"


/*

References:
https://learnopengl.com/Model-Loading/Model

*/


namespace NCL {
	namespace Rendering {
		
		

		class Model
		{

		public:
			vector<OGLTexture*>loadedTexture;
			vector<OGLMesh*>   meshes;
			std::string directory;
			bool gammaCorrection;
		

			// constructor, expects a filepath to a 3D model.
			Model(std::string const& path, bool gamma = false) : gammaCorrection(gamma)
			{
				//blackTexture = (OGLTexture*)TextureLoader::LoadAPITexture("blackPicture.jpg");
				loadModel(path);
			}
		
	
			~Model() {
				for (int i = 0; i < meshes.size(); i++)
				{
					delete meshes[i];

				}
			}

		private:
			// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
			OGLTexture* blackTexture = (OGLTexture*)TextureLoader::LoadAPITexture("blackPicture.jpg");
		
	

		
			void loadModel(std::string const& path)
			{
				// read file via ASSIMP
				Assimp::Importer importer;
				const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);
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

			OGLMesh* processMesh(aiMesh* mesh, const aiScene* scene)
			{
				
				// data to fill
				vector<Vertex> vertices;
				vector<unsigned int> indices;
				OGLMaterial* oglMaterial=nullptr;
				vector<OGLTexture*> pbrTexArry;
				for (size_t j = 0; j <TextureType::Max_Number; j++)
				{
					pbrTexArry.push_back(blackTexture);
				}
				aiMaterial* mat;
				

				for (unsigned int i = 0; i < mesh->mNumVertices; i++)
				{
					Vertex vertex;
					Vector3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
					// positions
					vector.x = mesh->mVertices[i].x;
					vector.y = mesh->mVertices[i].y;
					vector.z = mesh->mVertices[i].z;
					vertex.Position = vector;
					// normals
					vector.x = mesh->mNormals[i].x;
					vector.y = mesh->mNormals[i].y;
					vector.z = mesh->mNormals[i].z;
					vertex.Normal = vector;
					// texture coordinates
					if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
					{
						Vector2 vec;
						// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
						// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
						vec.x = mesh->mTextureCoords[0][i].x;
						vec.y = mesh->mTextureCoords[0][i].y;
						vertex.TexCoords = vec;
					}
					else
						vertex.TexCoords = Vector2(0.0f, 0.0f);
					// tangent
					vector.x = mesh->mTangents[i].x;
					vector.y = mesh->mTangents[i].y;
					vector.z = mesh->mTangents[i].z;
					vertex.Tangent = vector;
					// bitangent
					vector.x = mesh->mBitangents[i].x;
					vector.y = mesh->mBitangents[i].y;
					vector.z = mesh->mBitangents[i].z;
					vertex.Bitangent = vector;
					vertices.push_back(vertex);
				}
				// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
				for (unsigned int i = 0; i < mesh->mNumFaces; i++)
				{
					aiFace face = mesh->mFaces[i];
					// retrieve all indices of the face and store them in the indices vector
					for (unsigned int j = 0; j < face.mNumIndices; j++)
						indices.push_back(face.mIndices[j]);
				}
		
				mat = scene->mMaterials[mesh->mMaterialIndex];

				OGLTexture* diffuseMap = loadMaterialTextures(mat, aiTextureType_DIFFUSE);
				pbrTexArry[TextureType::ALBEDO_MAP] = diffuseMap;

				OGLTexture* normalMap = loadMaterialTextures(mat, aiTextureType_HEIGHT);
				pbrTexArry[TextureType::NORMAL_MAP] = normalMap;

				OGLTexture* metallicMap = loadMaterialTextures(mat, aiTextureType_UNKNOWN);
				pbrTexArry[TextureType::METALLIC_MAP] = metallicMap;

				OGLTexture* roughnesslMap = loadMaterialTextures(mat, aiTextureType_UNKNOWN);
				pbrTexArry[TextureType::ROUGHNESS_MAP] = roughnesslMap;

				OGLTexture* aolMap = loadMaterialTextures(mat, aiTextureType_UNKNOWN);
				pbrTexArry[TextureType::AO_MAP] = aolMap;

			

				//todo:material float
				//todo: 
				oglMaterial = new OGLMaterial(pbrTexArry);
				OGLMesh* Temp = NULL;
				Temp = new OGLMesh(vertices, indices, oglMaterial);
				
				return Temp;
			}

			
			 // checks all material textures of a given type and loads the textures if they're not loaded yet.
			// the required info is returned as a Texture struct.
			OGLTexture* loadMaterialTextures(aiMaterial* mat, aiTextureType type) {
			
				OGLTexture* texture=nullptr;
				aiString str;

					if (mat->GetTextureCount(type)>0) {
						mat->GetTexture(type, 0, &str);
						
						//todo:delete str.C_Str()  is null
						std::cout << "path" << Assets::TEXTUREDIR  << str.C_Str() << std::endl;

						for (int j = 0; j< loadedTexture.size(); j++)
						{	
							if (str.C_Str() == loadedTexture[j]->texPath) {
								texture = loadedTexture[j];
								//todo:delete str.C_Str()  is null
								std::cout << "already input" << std::endl;
								return texture;
							}
						}

						if (texture == nullptr)
						{
							texture = (OGLTexture*)TextureLoader::LoadAPITexture(str.C_Str());
							texture->texPath = str.C_Str();
							loadedTexture.push_back(texture);

						}

						texture = (OGLTexture*)TextureLoader::LoadAPITexture(str.C_Str());
						return texture;
						
					}
					else {
						texture = blackTexture;
						return texture;

					}
					return texture;
			}


		};
	}
};

