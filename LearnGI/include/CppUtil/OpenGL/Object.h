#ifndef SURFACE_H_
#define SURFACE_H_

namespace CppUtil {
	namespace OpenGL {
		class Shader;
		class DObject {
		public:
			DObject() {};
			virtual void Draw(const Shader& shader)const = 0;

		protected:
			bool isValid;
		private:

		};
	}
}
#endif // !SURFACE_H_