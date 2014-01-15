#ifndef _STATS_H_
#define _STATS_H_

//---------------------------------------------------------------------
struct KdTreeStats {
	int intersectionCalls;
	int sceneBBoxIntersections;
	int earlyOutCount;
	int nodesVisited;
	int leavesVisited;
	int interiorNodeCond0Count;
	int interiorNodeCond1Count;
	int interiorNodeCond2Count;
	int interiorNodeCond3Count;

	KdTreeStats()
		: intersectionCalls(0)
		, sceneBBoxIntersections(0)
		, earlyOutCount(0)
		, nodesVisited(0)
		, leavesVisited(0)
		, interiorNodeCond0Count(0)
		, interiorNodeCond1Count(0)
		, interiorNodeCond2Count(0)
		, interiorNodeCond3Count(0)
	{}
};

//---------------------------------------------------------------------
struct ShadingStats {
	int shadowRays;
	int shadowRaysPassed;

	ShadingStats()
		: shadowRays(0)
		, shadowRaysPassed(0)
	{}
};

//---------------------------------------------------------------------
struct Stats {
	static void Increment(int& value) {
		if (cfg_showConsole) {
			value += 1;
		}
	}
	static void Increment(int& value, int delta) {
		if (cfg_showConsole) {
			value += delta;
		}
	}
	KdTreeStats kdTreeStats;
	ShadingStats shadingStats;
};

#endif //_STATS_H_
