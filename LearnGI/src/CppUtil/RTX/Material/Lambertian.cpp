#include <CppUtil/RTX/Lambertian.h>
#include <CppUtil/RTX/ConstTexture.h>

using namespace RTX;
using namespace CppUtil::Basic;
using namespace glm;

Lambertian::Lambertian(Texture::CPtr albedo)
	: albedo(albedo) { }

Lambertian::Lambertian(float r, float g, float b)
	: Lambertian(rgb(r, g, b)) { }

Lambertian::Lambertian(const rgb& albedo) {
	this->albedo = ToPtr(new ConstTexture(albedo));
}