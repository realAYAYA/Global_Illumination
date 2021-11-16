#ifndef _MATERIAL_H_
#define _MATERIAL_H_
#include<CppUtil/Basic/HeapObj.h>
#include<CppUtil/RTX/MatVisitor.h>
#include<CppUtil/OpenGL/Mesh.h>// Vertex,vector

using CppUtil::OpenGL::Vertex;

#define MATERIAL_SETUP(CLASS) \
HEAP_OBJ_SETUP(CLASS)\
public:\
virtual void Accept(MatVisitor::Ptr matVisitor) const{\
	matVisitor->Visit(CThis());\
}

namespace RTX {
	class Material :public CppUtil::Basic::HeapObj {
		HEAP_OBJ_SETUP(Material)
	public:
		virtual void Accept(MatVisitor* matVisitor) const = 0;

		// return true means Ray continue spread
		// return false means Ray stop spread	
		//virtual bool Scatter(const HitRecord& rec) const = 0;// BRDF
	};
}

#endif // !_MATERIAL_H_

// 对宏的再封装，加入专门用于Material类对象的访问和转换