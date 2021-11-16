#ifndef _SCENE_H_
#define _SCENE_H_

#include <CppUtil/Basic/HeapObj.h>

namespace RTX {
	class Hitable;
	//class RayCamera;

	class RTScene : public CppUtil::Basic::HeapObj {
		HEAP_OBJ_SETUP(RTScene)
	public:
		const RTScene(CppUtil::Basic::CPtr<Hitable> obj);

		CppUtil::Basic::CPtr<Hitable> obj;
		//const CppUtil::Basic::CPtr<RayCamera> camera;
	};
}

#endif // !_SCENE_H_
