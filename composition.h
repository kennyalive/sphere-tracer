#ifndef _COMPOSITION_H_
#define _COMPOSITION_H_

#include "geometry.h"

class Object;
class CameraPose;

//---------------------------------------------------------------------
class Composition {
public:
	Composition(const std::string& name)
	: m_name(name) {
		m_next = m_sCompositions;
		m_sCompositions = this;
	}
	Composition* GetNextComposition() const {
		return m_next;
	}
	const std::string& GetName() const {
		return m_name;
	}

	static Composition* GetCompositions() {
		return m_sCompositions;
	}
	static Composition* FindComposition(const std::string& name) {
		Composition* comp = m_sCompositions;
		while (comp) {
			if (comp->m_name == name) {
				return comp;
			}
			comp = comp->m_next;
		}
		return NULL;
	}

	virtual CameraPose* GetCameraPose() { return NULL; }

	virtual void GetGeometryObjects(std::vector<Object*>& objects) = 0;
	virtual void GetLightObjects(std::vector<const Object*>& objects) = 0;
	virtual void Update(float frameTime) = 0;

private:
	std::string m_name;
	Composition* m_next;
	static Composition* m_sCompositions;
};

#endif //_COMPOSITION_H_
