#include<CppUtil/OpenGL/Scene.h>
#include<CppUtil/OpenGL/Shader.h>
#include<CppUtil/OpenGL/TfmObject.h>
#include<CppUtil/OpenGL/Skybox.h>

void CppUtil::OpenGL::Scene::Push(TfmObject* _obj)
{
	if (_obj->IsValid()) {
		this->objects.push_back(_obj);
	}
}

void CppUtil::OpenGL::Scene::Draw(Shader& shader) const
{
	for (int i = 0; i < this->objects.size(); i++) {
		objects[i]->Draw(shader);
	}
	if (this->skybox) {
		
	}
}