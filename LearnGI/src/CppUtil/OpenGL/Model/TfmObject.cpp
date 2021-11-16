#include<CppUtil/OpenGL/TfmObject.h>
#include<CppUtil/OpenGL/Shader.h>

CppUtil::OpenGL::TfmObject::TfmObject(DObject* _obj, const glm::mat4& _transform)
{
	this->transform = _transform;
	this->object = _obj;
	if (this->object != nullptr) {
		this->isValid = true;
	}
	else {
		this->isValid = false;
	}
}

void CppUtil::OpenGL::TfmObject::Draw(const Shader& shader)const
{
	/* Set View and Projection Matrices here if you want*/
	shader.SetMat4f("model", transform);
	this->object->Draw(shader);
}