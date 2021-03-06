#include "RenderObject.h"
//#include "../../Common/MeshGeometry.h"
//#include "..\..\Plugins\OpenGLRendering\OGLMesh.h"
#include "..\..\Plugins\OpenGLRendering\OGLMesh.h"
#include "..\..\Plugins\OpenGLRendering\OGLMaterial.h"
using namespace NCL::CSC8503;
using namespace NCL;

RenderObject::RenderObject(Transform* parentTransform, OGLMesh* mesh, TextureBase* colourtex, ShaderBase* shader) {
	this->IsPBR = false;
	this->transform	= parentTransform;
	this->mesh		= mesh;
	this->texture	= colourtex;
	this->shader	= shader;
	this->colour	= Vector4(1.0f, 1.0f, 1.0f, 1.0f);

}

RenderObject::RenderObject(Transform* parentTransform, OGLMesh* mesh, OGLMaterial* meterial, ShaderBase* pbrShader, ShaderBase* FengShader, bool isPBR) {
	this->IsPBR = isPBR;
	if(isPBR){
	this->texture = meterial->pbrTexArry[ALBEDO_MAP];
	this->transform = parentTransform;
	this->mesh = mesh;
	this->material = meterial;
	this->shader = pbrShader;
	this->colour = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	else {
		this->transform = parentTransform;
		this->mesh = mesh;
		this->texture = meterial->pbrTexArry[ALBEDO_MAP];
		this->shader = FengShader;
		this->colour = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	
	}
}

RenderObject::~RenderObject() {

}