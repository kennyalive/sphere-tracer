#include "stdafx.h"
#include "kdtree.h"
#include "object.h"
#include "stats.h"

//---------------------------------------------------------------------
void KdTree::BuildTree(const std::vector<Object*>& objects) {
	m_nodes.resize(0);
	m_leafObjects.resize(0);
	m_tempObjects.resize(0);
	m_edges.resize(0);
	m_bounds = BBox();
	m_nodesPtr = NULL;

	if (objects.empty()) {
		return;
	}
	// Compute the bounds of all objects
	for (size_t i = 0; i < objects.size(); i++) {
		m_bounds.AddBBox(objects[i]->GetBounds());
	}
	// Allocate working memory and recursively create nodes
	const int maxDepth = int(8 + 1.3f * Log2Int(float(objects.size())));
	//const int maxDepth = 0;
	m_tempObjects.resize(objects.size() * (maxDepth+1));
	m_edges.resize(objects.size() * 2);
	BuildNode(m_bounds, &objects[0], (int)objects.size(), maxDepth, &m_tempObjects[0], &m_tempObjects[0] + objects.size());
	m_nodesPtr = &m_nodes[0];
}

//---------------------------------------------------------------------
void KdTree::CreateLeaf(const Object* const* objects, int numObjects) {
	m_nodes.push_back(KdNode());
	KdNode& node = m_nodes.back();
	node.type = KD_LEAF;
	node.firstObject = (numObjects > 0) ? (int)m_leafObjects.size() : -1;
	node.numObjects = numObjects;
	m_leafObjects.insert(m_leafObjects.end(), objects, objects + numObjects);
}

//---------------------------------------------------------------------
void KdTree::BuildNode(const BBox& nodeBounds, const Object* const* objects, int numObjects, int depth,
							 const Object** objects0, const Object** objects1)
{
	if (numObjects == 1 || !depth) {
		CreateLeaf(objects, numObjects);
		return;
	}
	// Select split axis
	Vec3 d = nodeBounds.pMax - nodeBounds.pMin;
	int axis = (d.x > d.y && d.x > d.z) ? 0 : (d.y > d.z ? 1 : 2);
	// Init edges
	m_edges.resize(numObjects * 2);
	for (int i = 0; i < numObjects; i++) {
		const Object* s = objects[i];
		BBox bounds = s->GetBounds();
		m_edges[2*i + 0] = BoundEdge(bounds.pMin[axis], s, true);
		m_edges[2*i + 1] = BoundEdge(bounds.pMax[axis], s, false);
	}
	std::sort(m_edges.begin(), m_edges.end());
	// Select split position
	int splitOffset;
	if (!SelectSplitPosition(nodeBounds, axis, splitOffset)) {
		CreateLeaf(objects, numObjects);
		return;
	}
	const float split = m_edges[splitOffset].t;
	// Classify objects with respect to split
	int n0 = 0;
	for (int i = 0; i < splitOffset; i++) {
		if (m_edges[i].type == BoundEdge::START && m_edges[i].t < split)
			objects0[n0++] = m_edges[i].obj;
	}
	int n1 = 0;
	for (int i = splitOffset+1; i < numObjects*2; i++) {
		if (m_edges[i].type == BoundEdge::END && m_edges[i].t > split)
			objects1[n1++] = m_edges[i].obj;
	}
	// Add interior node and recursively create children nodes
	int nodeIndex = (int)m_nodes.size();
	m_nodes.push_back(KdNode());
	m_nodes[nodeIndex].type = static_cast<KdNodeType>(KD_X_AXIS + axis);
	m_nodes[nodeIndex].split = split;

	BBox bounds0 = nodeBounds;
	bounds0.pMax[axis] = split;
	BuildNode(bounds0, objects0, n0, depth-1, objects0, objects1 + numObjects);
	m_nodes[nodeIndex].aboveChild = (int)m_nodes.size();

	BBox bounds1 = nodeBounds;
	bounds1.pMin[axis] = split;
	BuildNode(bounds1, objects1, n1, depth-1, objects0, objects1);
}

//---------------------------------------------------------------------
bool KdTree::SelectSplitPosition(const BBox& nodeBounds, int axis, int& splitOffset) {
	const int otherAxis[3][2] = { {1,2}, {0,2}, {0,1} };
	const int otherAxis0 = otherAxis[axis][0];
	const int otherAxis1 = otherAxis[axis][1];
	const Vec3 d = nodeBounds.pMax - nodeBounds.pMin;
	const float s0 = 2.0f * (d[otherAxis0] * d[otherAxis1]);
	const float d0 = 2.0f * (d[otherAxis0] + d[otherAxis1]);
	const float invTotalS = 1.f / (2.f * (d.x*d.y + d.x*d.z + d.y*d.z));

	int   numBelow = 0;
	int	  numAbove = (int)m_edges.size() / 2;
	float bestCost = m_isectCost * numAbove;
	int   bestOffset = -1;
	// Iterate for all possible splits
	for (int i = 0; i < (int)m_edges.size(); i++) {
		if (m_edges[i].type == BoundEdge::END) {
			numAbove--;
		}
		float t = m_edges[i].t;
		if (t > nodeBounds.pMin[axis] && t < nodeBounds.pMax[axis]) {
			// Compute cost for split i
			float belowS = s0 + d0*(t - nodeBounds.pMin[axis]);
			float aboveS = s0 + d0*(nodeBounds.pMax[axis] - t);
			float pBelow = belowS * invTotalS;
			float pAbove = aboveS * invTotalS;
			float eb = (!numBelow || !numAbove) ? m_emptyBonus : 0.0f;
			float cost = m_traversalCost + m_isectCost*(1.0f - eb)*(pBelow*numBelow + pAbove*numAbove);
			// Update best cost
			if (cost < bestCost) {
				bestCost = cost;
				bestOffset = i;
			}
		}
		if (m_edges[i].type == BoundEdge::START) {
			numBelow++;
		}
	}
	if (bestOffset == -1) {
		return false;
	}
	splitOffset = bestOffset;
	return true;
}

//---------------------------------------------------------------------
const Object* KdTree::Intersect(const Ray& ray, KdTreeStats& stats) const {
	Stats::Increment(stats.intersectionCalls);
	if (!m_nodesPtr) {
		return NULL;
	}
	// Initialize ray segment
	float tmin, tmax;
	if (!m_bounds.Intersect(ray, tmin, tmax)) {
		return false;
	}
	Stats::Increment(stats.sceneBBoxIntersections);

	// TraversalInfo is used to recursively traverse kd-tree
	struct TraversalInfo {
		TraversalInfo() {}
		TraversalInfo(const KdNode* n, float tmin_, float tmax_) 
			: node(n), tmin(tmin_), tmax(tmax_)
		{}
		const KdNode* node;
		float tmin, tmax;
	};
	const int TRAVERSAL_MAX_DEPTH = 64; // I'm sure, that's sufficient for everything :)
	TraversalInfo traversalStack[TRAVERSAL_MAX_DEPTH];
	int traversalStackSize = 0;

	const KdNode* node = m_nodesPtr;
	const Object* closestObject = NULL;
	// Let's traverse it!
	while (true) {
		if (ray.tmax < tmin) {
			Stats::Increment(stats.earlyOutCount);
			break;
		}
		Stats::Increment(stats.nodesVisited);
		// Process leaf
		if (node->type == KD_LEAF) {
			Stats::Increment(stats.leavesVisited);
			for (int i = 0; i < node->numObjects; i++) {
				const Object* s = m_leafObjects[node->firstObject + i];
				if (s->Intersect(ray)) {
					closestObject = s;
				}
			}
			if (!traversalStackSize) {
				break;
			}
			const TraversalInfo& ti = traversalStack[--traversalStackSize];
			node = ti.node;
			tmin = ti.tmin;
			tmax = ti.tmax;
		}
		// Process interior node
		else {
			const int axis = node->type;
			const float delta = node->split - ray.origin[axis];
			if (!delta) {
				if (ray.dir[axis] > 0.f) {
					node = &m_nodesPtr[node->aboveChild];
					tmin = std::max(0.f, tmin);
				}	 
				else if (ray.dir[axis] < 0.f) {
					node = node + 1;
					tmin = std::max(0.f, tmin);
				}
				else { // ray.dir[axis] == 0.f
					node = node + 1; // It doesn't matter which node we select here
				}
			}
			else
			{
				const KdNode* firstChild, *secondChild;
				if (delta > 0.f) {
					firstChild = node + 1;
					secondChild = &m_nodesPtr[node->aboveChild];
				}
				else { // delta < 0
					assert(delta < 0.f);
					firstChild = &m_nodesPtr[node->aboveChild];
					secondChild = node + 1;
				}
				const float tsplit = delta * ray.invDir[axis];

				if (tsplit >= tmax) {
					node = firstChild;
					Stats::Increment(stats.interiorNodeCond0Count);
				}
				else if (tsplit < 0) {
					if (tsplit > tmin) {
						tmin = tsplit;
					}
					node = firstChild;
					Stats::Increment(stats.interiorNodeCond1Count);
				}
				else if (tmin >= tsplit) {
					node = secondChild;
					Stats::Increment(stats.interiorNodeCond2Count);
				}
				else {
					assert(traversalStackSize != TRAVERSAL_MAX_DEPTH);
					assert(tsplit > tmin && tsplit < tmax);
					traversalStack[traversalStackSize++] = TraversalInfo(secondChild, tsplit, tmax);
					node = firstChild;
					tmax = tsplit;
					Stats::Increment(stats.interiorNodeCond3Count);
				}
			}
		}
	}
	ray.tmax -= 1e-3f;
	return closestObject;
}
