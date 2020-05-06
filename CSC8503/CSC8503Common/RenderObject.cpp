#include "RenderObject.h"
//#include "../../Common/MeshGeometry.h"
//#include "..\..\Plugins\OpenGLRendering\OGLMesh.h"
#include "..\..\Plugins\OpenGLRendering\OGLMesh.h"
using namespace NCL::CSC8503;
using namespace NCL;

RenderObject::RenderObject(Transform* parentTransform, OGLMesh* mesh, TextureBase* tex, ShaderBase* shader) {
	this->transform	= parentTransform;
	this->mesh		= mesh;
	this->texture	= tex;
	this->shader	= shader;
	this->colour	= Vector4(1.0f, 1.0f, 1.0f, 1.0f);
}

RenderObject::~RenderObject() {

}