#pragma once

#include <string>
#include <vector>
#include "../../Common/Vector2.h"
#include "../../Common/Vector3.h"
#include "../../Common/Vector4.h"
#include "OGLMesh.h"

/*
reference:
https://learnopengl.com/Model-Loading/Mesh
*/

namespace NCL {
	namespace Rendering {
		struct Vertex {
			// position
			Vector3 Position;
			// normal
			Vector3 Normal;
			// texCoords
			Vector2 TexCoords;
			// tangent
			Vector3 Tangent;
			// bitangent
			Vector3 Bitangent;
		};

		struct Texture {
			unsigned int id;
			std::string type;
			std::string path;
		};
		class Mesh :public OGLMesh
		{
		public:
			// mesh Data
			vector<Vertex>       vertices;
			vector<unsigned int> indices;
			vector<Texture>      textures;
			//todo: unsigned int VAO;

			// constructor
			Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
			{
				//
				vao = 0;
				subCount = 1;

				for (int i = 0; i < MAX_BUFFER; ++i) {
					buffers[i] = 0;
				}


				this->vertices = vertices;
				this->indices = indices;
				this->textures = textures;

				// now that we have all the required data, set the vertex buffers and its attribute pointers.
				setupMesh();
			}
		private:
			void setupMesh()
			{



			}


		};
	}
}

