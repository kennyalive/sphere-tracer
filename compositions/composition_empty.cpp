#include "stdafx.h"
#include "../composition.h"

//---------------------------------------------------------------------
class Composition_Empty : public Composition {
public:
	Composition_Empty()
	: Composition("empty")
	{}

	virtual void GetGeometryObjects(std::vector<Object*>& objects) {}
	virtual void GetLightObjects(std::vector<const Object*>& objects) {}
	virtual void Update(float frameTime) {}

private:
};

static Composition_Empty compositionObject;
