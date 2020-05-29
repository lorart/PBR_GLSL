/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
/*
reference:
https://learnopengl.com/Model-Loading/Mesh
*/
#pragma once
#include "../../Common/MeshGeometry.h"
#include "glad\glad.h"

#include <string>
#include"OGLMaterial.h"

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


		enum TextureType {
			ALBEDO_MAP,
			NORMAL_MAP,
			METALLIC_MAP,
			ROUGHNESS_MAP,
			AO_MAP,
			Max_Number
		};

		class OGLMesh : public NCL::MeshGeometry
		{
		public:

			enum MeshBuffer {
				VERTEX_BUFFER,
				COLOUR_BUFFER,
				TEXTURE_BUFFER,
				NORMAL_BUFFER,
				TANGENT_BUFFER,
				BITANGENT_BUFFER,
				INDEX_BUFFER,
				SKELINDEX_BUFFER,
				SKELWEIGHT_BUFFER,
				COMBINE_BUFFER,
				MAX_BUFFER
			};

			friend class OGLRenderer;
			OGLMesh();
			OGLMesh(const std::string&filename);
		
			~OGLMesh();

			void RecalculateNormals();

			void UploadToGPU() override;

		protected:
			
			GLuint	GetVAO()			const { return vao;			}
			int		GetSubMeshCount()	const { return subCount;	}

			void BindVertexAttribute(int attribSlot, int bufferID, int bindingID, int elementCount, int elementSize, int elementOffset);

			//void BufferData();

			int		subCount;

			GLuint ebo;
			GLuint vao;
			//GLuint oglType;
			GLuint buffers[MAX_BUFFER];


			//for assimp library
		public:
			// mesh Data
			vector<Vertex>       vertices;
			vector<unsigned int> indices;
			
			//todo: unsigned int VAO;
			OGLMesh(vector<Vertex> vertices, vector<unsigned int> indices);
			OGLMesh(vector<Vertex> vertices, vector<unsigned int> indices, OGLMaterial* material);
			bool getIsAsimmp() {
				return this->IsAsimmp;
			}
			OGLMaterial* material;
		private:
			bool IsAsimmp;

		};
	}
}