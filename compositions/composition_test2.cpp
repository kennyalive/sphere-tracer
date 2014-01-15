#include "stdafx.h"
#include "../composition.h"
#include "../object.h"

//---------------------------------------------------------------------
class Composition_Test2 : public Composition {
public:
	Composition_Test2()
	: Composition("test2")
	, m_ground(Vec3(0, 5, -2), 2.f, 0.7f, 0.f)
	, m_planet(Vec3(0, 4, 1), 0.5f, 0.8f, 0.f)
	, m_satellite(Vec3(0, 2, 1), 0.1f, 0.8f, 0.f)
	, m_animPhase(0)
	, m_satelliteZAngle(0)
	, m_satteliteXAngle(0)
	{
		m_lights[0] = Object(Vec3(0, 5, 6), 0.1f, 0.f, 200.f);
		m_lights[1] = Object(Vec3(-2.5f, 3, 6), 0.1f, 0.f, 200.f);
		m_lights[2] = Object(Vec3(0, 0, -2), 0.1f, 0.f, 40.f);
	}
	virtual void GetGeometryObjects(std::vector<Object*>& objects) {
		objects.push_back(&m_ground);
		objects.push_back(&m_planet);
		objects.push_back(&m_satellite);
	}
	virtual void GetLightObjects(std::vector<const Object*>& objects) {
		objects.push_back(&m_lights[0]);
		objects.push_back(&m_lights[1]);
		objects.push_back(&m_lights[2]);
	}
	virtual void Update(float frameTime) {
		if (m_animPhase == 0) {
			m_planet.center.x += frameTime * 0.2f;
			if (m_planet.center.x > 1.5f)
				m_animPhase = 1;
		}
		else {
			m_planet.center.x -= frameTime * 0.5f;
			if (m_planet.center.x < -1.0f)
				m_animPhase = 0;
		}
		const float r = m_planet.radius + 0.3f;
		Vec3 center = m_planet.center;
		m_satelliteZAngle += frameTime * 110.0f;
		m_satteliteXAngle += frameTime * 75.0f;

		Mat3 rotZ;
		rotZ.SetRotationZ(m_satelliteZAngle);

		Mat3 rotX;
		rotX.SetRotationX(m_satteliteXAngle);

		Vec3 sattelitePos = rotX * rotZ * Vec3(r, 0, 0);
		m_satellite.center = center + sattelitePos;
	}

private:
	Object m_ground;
	Object m_planet;
	Object m_satellite;
	Object m_lights[3];
	int m_animPhase;
	float m_satelliteZAngle;
	float m_satteliteXAngle;
};

static Composition_Test2 compositionObject;
