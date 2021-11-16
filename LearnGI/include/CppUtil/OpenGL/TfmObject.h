#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include<CppUtil/OpenGL/Model.h>

namespace CppUtil {
	namespace OpenGL {
		class Shader;
		class DObject;
		class TfmObject:public DObject {
		public:
			TfmObject() { this->isValid = false; }
			TfmObject(DObject* _obj, const glm::mat4& _transform = glm::mat4(1));
			virtual void Draw(const Shader& shader)const override;
			bool IsValid()const { return this->isValid; }

			void SetTransform(const glm::mat4& _transform) { transform = _transform; }
			void SetWorldLocation(const glm::vec3& pos) { worldLocation = pos; }

			glm::mat4 GetTransform()const { return this->transform; }
			glm::vec3 GetWorldLocation()const { return this->worldLocation; }
			glm::vec3 GetWorldRotation()const { return this->rotate; }
			glm::vec3 GetScale()const { return this->scale; }

		private:
			glm::mat4 transform;

			glm::vec3 worldLocation;
			glm::vec3 rotate;
			glm::vec3 scale;

			DObject* object;
			bool isValid;
		};
	}
}

#endif// ! _TRANSFORM_H_