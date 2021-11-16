#include <CppUtil/RTX/RTScene.h>

using namespace RTX;
using namespace CppUtil::Basic;

RTScene::RTScene(CppUtil::Basic::CPtr<Hitable> obj)
	: obj(obj) { }