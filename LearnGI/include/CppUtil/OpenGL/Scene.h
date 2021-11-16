#ifndef _GLSCENE_H_
#define _GLSCENE_H_

#include<vector>
using std::vector;

namespace CppUtil {
	namespace OpenGL {
		class Shader;
		class TfmObject;
		class Skybox;
		class Scene {
		public:
			Scene() { this->IsValid = true; }
			
			void Draw(Shader& shader)const;

			void Push(TfmObject* _obj);

		private:
			vector<TfmObject*> objects;
			Skybox* skybox;

			bool IsValid;
		};
	}
}

#endif// ! _GLSCENE_H_