#ifndef MESH_H_
#define MESH_H_
#include<string>
#include<vector>

#include<gl/glew.h>
#include<glm/glm.hpp>

#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>

#include<CppUtil/OpenGL/Object.h>

namespace CppUtil {
	namespace OpenGL {
		struct Vertex {
			Vertex(glm::vec3 pos = glm::vec3(0), glm::vec3 normal = glm::vec3(0, 0, 1), float u = 0, float v = 0);

			glm::vec3 position;
			glm::vec3 normal;
			glm::vec2 texCoords;

			void Transform(const glm::mat4& transform);
			void Transform(const glm::mat4& transform, const glm::mat3& normalTransform);
			static const Vertex Interpolate_Tri(const glm::vec3& abg, const Vertex& A, const Vertex& B, const Vertex& C);
		};

		struct TextureInfo {
			unsigned int id;
			std::string type;
			aiString path;
		};

		using std::vector;

		class Shader;
		
		class Mesh:public DObject
		{
		public:
			Mesh() {};
			Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<TextureInfo> textures);
			~Mesh();			

			virtual void Draw(const Shader& shader)const override;
			//void DrawArrays(Shader* shader, const glm::mat4& modelMat);
			//void DrawElements(Shader* shader);

			unsigned int GetVAO()const { return this->VAO; }
		private:
			unsigned int VAO;
			vector<Vertex> vertices;
			vector<unsigned int> indices;
			vector<TextureInfo> textures;

			void setupMesh();
		};
	}
}
#endif // !MESH_H_