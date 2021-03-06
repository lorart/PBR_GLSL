/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#pragma once
#include "../../Common/TextureBase.h"
#include "glad\glad.h"

#include <string>
#include <vector>
//textureRelate
namespace NCL {
	namespace Rendering {
		class OGLTexture : public TextureBase
		{
		public:
			//friend class OGLRenderer;
			 OGLTexture();
			 OGLTexture(GLuint texToOwn);
			~OGLTexture();

			static TextureBase* RGBATextureFromData(char* data, int width, int height, int channels);


			static TextureBase* RGBATextureFromFilename(const std::string&name);

			static TextureBase* LinerRGBATextureFromData(char* data, int width, int height, int channels);

			static TextureBase* AllocateCubeTexture(int size);

			static TextureBase* AllocateCubeTexture_Midmap(int size);
			static  TextureBase* LinerRGBATextureFromFilename(const std::string& name);


		
			std::string texPath;
			GLuint GetObjectID() const	{
				return texID;
			}
		protected:						
			GLuint texID;


		};
	}
}

