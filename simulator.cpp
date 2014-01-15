#include "stdafx.h"
#include "simulator.h"
#include "scene.h"
#include "object.h"

//---------------------------------------------------------------------
Simulator::Simulator()
: m_currentTime(0.f)
, m_collisionTime(FLT_MAX)
, m_collObject0(NULL)
, m_collObject1(NULL)
{
	m_gridDims[0] = m_gridDims[1] = m_gridDims[2] = 0;
	m_gridStep = 0.f;
}

//---------------------------------------------------------------------
void Simulator::SetObjects(const std::vector<Object*>& objects) {
	m_objects = objects;
	InitGrid();
	FindNextCollision();
}

//---------------------------------------------------------------------
void Simulator::DoStep(float frameTime) {
	float nextFrameTime = m_currentTime + frameTime;
	if (nextFrameTime < m_collisionTime) {
		UpdatePositions(frameTime);
		m_currentTime = nextFrameTime;
	}
	else {
		const float eps = 1e-4;
		float dt = m_collisionTime - m_currentTime;
		while (frameTime > eps) {
			UpdatePositions(dt);
			ProcessCollision();
			frameTime -= dt;
			FindNextCollision();
			if (m_collisionTime > frameTime || m_collisionTime == FLT_MAX) {
				UpdatePositions(frameTime);
				m_currentTime = frameTime;
				break;
			}
			else {
				dt = m_collisionTime;
				m_currentTime = 0.f;
			}
		}
	}
}

//---------------------------------------------------------------------
void Simulator::UpdatePositions(float dt) {
	for (size_t i = 0; i < m_objects.size(); i++) {
		Object* s = m_objects[i];
		if (/*s->mass &&*/ !s->animated) {
			s->center += s->velocity * dt;
		}
	}
}

//---------------------------------------------------------------------
static float CheckCollision(const Object* s0, const Object* s1) {
	const Vec3 c = s1->center - s0->center;
	const Vec3 v = s1->velocity - s0->velocity;
	const float d = s0->radius + s1->radius;

	const float A = Dot(v, v);
	const float B = 2.f*Dot(c, v);
	const float C = Dot(c, c) - d*d;

	float D = B*B - 4*A*C;
	if (D < 0.f) {
		return FLT_MAX;
	}
	if (D == 0.f) {
		return -B/(2.f*A);
	}
	else {
		D = sqrtf(D);
		float t = (-B - D)/(2.f*A);
		if (t < 0) {
			t = (-B + D)/(2.f*A);
		}
		if (t < 0) {
			return FLT_MAX;
		}
		return t;
	}
}

//---------------------------------------------------------------------
void Simulator::FindNextCollision() {
	float minTime = FLT_MAX;
	int bestO0 = -1;
	int bestO1 = -1;
	const int n = m_objects.size();

	// Check collision between objects
	for (size_t i = 0; i < n-1; i++) {
		const Object* obj0 = m_objects[i];
		for (size_t k = i+1; k < n; k++) {
			const Object* obj1 = m_objects[k];
			float t = CheckCollision(obj0, obj1);
			if (t < minTime) {
				minTime = t;
				bestO0 = i;
				bestO1 = k;
			}
		}
	}
	m_collisionTime = minTime;
	if (bestO0 != -1) {
		m_collObject0 = m_objects[bestO0];
	}
	if (bestO1 != -1) {
		m_collObject1 = m_objects[bestO1];
	}
}

//---------------------------------------------------------------------
void Simulator::ProcessCollision() {
	const Vec3  delta = m_collObject0->center - m_collObject1->center;
	const float deltaLen = delta.Length();
	const float dist = m_collObject0->radius + m_collObject1->radius;
	const float eps = 1e-3;
	assert(deltaLen <= dist + 1e-3 && deltaLen >= dist - 1e-3);

	const Vec3 v0 = m_collObject0->velocity;
	const Vec3 v1 = m_collObject1->velocity;
	const float m0 = m_collObject0->mass;
	const float m1 = m_collObject1->mass;

	if (!m_collObject0->mass && !m_collObject1->mass) {
		m_collObject0->velocity = v1;
		m_collObject1->velocity = v0;
	}
	else if (!m_collObject0->mass) {
		m_collObject0->velocity = v0;
		m_collObject1->velocity = 2.0f * v0 - v1;
	}
	else if (!m_collObject1->mass) {
		m_collObject0->velocity = 2.0f * v1 - v0;
		m_collObject1->velocity = v1;
	}
	else {
		m_collObject0->velocity = -v0 + 2.0f * (m0*v0 + m1*v1) / (m0 + m1);
		m_collObject1->velocity = -v1 + 2.0f * (m0*v0 + m1*v1) / (m0 + m1);
	}
	m_collObject0->center = m_collObject1->center + delta * ((dist + eps) / deltaLen);

	m_collObject0 = NULL;
	m_collObject1 = NULL;
	m_collisionTime = FLT_MAX;
}

//---------------------------------------------------------------------
void Simulator::InitGrid() {
	BBox b;
	for (size_t i = 0; i < m_objects.size(); i++) {
		b.AddBBox(m_objects[i]->GetBounds());
	}
	const float extraSize = 2.0f;
	b.pMin -= Vec3(extraSize);
	b.pMax += Vec3(extraSize);

	const Vec3 d = b.pMax - b.pMin;
	const int n0 = int(powf(m_objects.size(), 1.f/3.f)) + 2;

	int axis[3] = { 0, 1, 2 };
	for (int i = 0; i < 3; i++) {
		for (int k = i+1; k < 3; k++) {
			if (d[axis[k]] < d[axis[i]])
				std::swap(axis[k], axis[i]);
		}
	}
	const int n1 = int(d[axis[1]] / d[axis[0]] * n0);
	const int n2 = int(d[axis[2]] / d[axis[0]] * n0);

	m_gridStep = d[axis[0]] / n0;
	m_gridDims[axis[0]] = n0;
	m_gridDims[axis[1]] = n1;
	m_gridDims[axis[2]] = n2;

	const Vec3 center = (b.pMin + b.pMax) * 0.5f;
	const Vec3 d2 = Vec3(m_gridDims[0], m_gridDims[1], m_gridDims[2]) * m_gridStep / 2.0f;
	m_gridBounds.pMin = center - d2;
	m_gridBounds.pMax = center + d2;

	m_grid.resize(n0 * n1 * n2);
	UpdateGrid();
}

//---------------------------------------------------------------------
void Simulator::UpdateGrid() {
	/*for (size_t i = 0; i < m_objects.size(); i++) {
	}*/
}
