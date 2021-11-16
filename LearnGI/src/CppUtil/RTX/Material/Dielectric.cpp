#include<CppUtil/RTX/Dielectric.h>

using namespace RTX;

RTX::Dielectric::Dielectric(float refractIndex, glm::vec3 attenuationConst)
	: refractIndex(refractIndex), attenuationConst(attenuationConst) { }
