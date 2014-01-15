#ifndef _KDTREE_H_
#define _KDTREE_H_

#include "geometry.h"
class Object;
struct KdTreeStats;

enum KdNodeType {
	KD_X_AXIS = 0,
	KD_Y_AXIS = 1,
	KD_Z_AXIS = 2,
	KD_LEAF
};

struct KdNode {
	KdNodeType type;
	// Interior node
	float split;
	int aboveChild;
	// Leaf node
	int firstObject, numObjects; // Range in KdTree::m_leafObjects vector
};

struct BoundEdge {
	BoundEdge() { }
	BoundEdge(float tt, const Object* o, bool starting) {
		t = tt;
		obj = o;
		type = starting ? START : END;
	}
	bool operator < (const BoundEdge& e) const {
		if (t == e.t)
			return (int)type < (int)e.type;
		else 
			return t < e.t;
	}
	float t;
	const Object* obj;
	enum { START, END } type;
};

//---------------------------------------------------------------------
class KdTree {
public:
	KdTree(float traversalCost, float isectCost, float emptyBonus)
		: m_traversalCost(traversalCost)
		, m_isectCost(isectCost)
		, m_emptyBonus(emptyBonus)
		, m_nodesPtr(NULL)
	{}
	void BuildTree(const std::vector<Object*>& objects);
	const Object* Intersect(const Ray& ray, KdTreeStats& stats) const;

private:
	void CreateLeaf(const Object* const* objects, int numObjects);

	void BuildNode(const BBox& nodeBounds, const Object* const* objects, int numObjects, int depth,
								 const Object** objects0, const Object** objects1);

	bool SelectSplitPosition(const BBox& nodeBounds, int axis, int& splitOffset);

private:
	const float m_traversalCost;
	const float m_isectCost;
	const float m_emptyBonus;

	BBox                       m_bounds; // Bounds of all objects
	std::vector<KdNode>        m_nodes;       // Nodes of Kd-tree
	const KdNode*			   m_nodesPtr;
	std::vector<const Object*> m_leafObjects; // Leaf nodes store their objects in this vector
	std::vector<const Object*> m_tempObjects; // Used during Kd-tree construction
	std::vector<BoundEdge>     m_edges;
};

#endif //_KDTREE_H_
