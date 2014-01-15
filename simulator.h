#ifndef _SIMULATOR_H_
#define _SIMULATOR_H_

#include "geometry.h"
class Object;

//---------------------------------------------------------------------
class Simulator {
public:
	Simulator();
	void SetObjects(const std::vector<Object*>& objects);
	void DoStep(float frameTime);

private:
	void UpdatePositions(float dt);
	void FindNextCollision();
	void ProcessCollision();

	void InitGrid();
	void UpdateGrid();

private:
	// Simulated objects
	std::vector<Object*> m_objects;

	// Collision
	float m_currentTime;		// From collision time track
	float m_collisionTime;		// From collision time track
	Object* m_collObject0;		// Collided object
	Object* m_collObject1;		// Collided object

	// Grid accelerator
	std::vector<std::vector<Object*> > m_grid;
	BBox m_gridBounds;
	int m_gridDims[3];
	float m_gridStep;
};

#endif //_SIMULATOR_H_
