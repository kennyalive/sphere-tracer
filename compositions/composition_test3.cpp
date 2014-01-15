#include "stdafx.h"
#include "../composition.h"
#include "../object.h"
#include "../camera.h"

//---------------------------------------------------------------------
class Composition_Test3 : public Composition {
public:
	Composition_Test3()
		: Composition("test3")
		, m_light(Vec3(0, 0, 10), 0.1f, 0.f, 1000.f)
		, m_light2(Vec3(-3, -13, 2), 0.1f, 0.f, 250.f)
	{
		m_objects.push_back(Object(Vec3(0, -3, 0.5), 2.0f, 0.7f, 0.f));

		const float r = 1000.f;
		m_objects.push_back(Object(Vec3(0, 0, -r-1), r, 0.2f, 0.f));
		m_objects.push_back(Object(Vec3(0, 0, r+12), r, 0.2f, 0.f));
		m_objects.push_back(Object(Vec3(-r-5, 0, 0), r, 0.2f, 0.f));
		m_objects.push_back(Object(Vec3(r+5, 0, 0), r, 0.2f, 0.f));
		m_objects.push_back(Object(Vec3(0, r+5, 0), r, 0.2f, 0.f));
		m_objects.push_back(Object(Vec3(0, -r-16, 0), r, 0.2f, 0.f));
	}

	virtual CameraPose* GetCameraPose() {
		static StdCameraPose pose(Vec3(0, -15.5, 2), Mat3::identity);
		return &pose;
	}
	virtual void GetGeometryObjects(std::vector<Object*>& objects) {
		for (int i = 0; i < m_objects.size(); i++) {
			objects.push_back(&m_objects[i]);
		}
	}
	virtual void GetLightObjects(std::vector<const Object*>& objects) {
		objects.push_back(&m_light);
		objects.push_back(&m_light2);
	}
	virtual void Update(float frameTime) {}

private:
	std::vector<Object> m_objects;
	Object m_light;
	Object m_light2;
};

static Composition_Test3 compositionObject;
