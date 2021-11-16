#include<CppUtil/RTX/Isotropic.h>
#include <CppUtil/RTX/ConstTexture.h>

using namespace RTX;
using namespace CppUtil::Basic;

Isotropic::Isotropic(const glm::rgb& color)
	: texture(ToPtr(new ConstTexture(color))) { }