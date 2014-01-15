#include "stdafx.h"
#include "../composition.h"
#include "../object.h"

//---------------------------------------------------------------------
class Composition_Test1 : public Composition {
public:
	Composition_Test1()
	: Composition("test1")
	, m_sphere(Vec3(0, 10, 0), 6.f, 0.7f, 0.f)
	, m_light(Vec3(0, -2, 0), 0.1f, 0.f, 450.f)
	{}

	virtual void GetGeometryObjects(std::vector<Object*>& objects) {
		objects.push_back(&m_sphere);
	}
	virtual void GetLightObjects(std::vector<const Object*>& objects) {
		objects.push_back(&m_light);
	}
	virtual void Update(float frameTime) {}

private:
	Object m_sphere;
	Object m_light;
};

static Composition_Test1 compositionObject;
