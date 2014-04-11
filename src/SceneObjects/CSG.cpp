#include <cmath>
#include <assert.h>
#include <cfloat>



#include "CSG.h"

Segments& Segments::Merge(const Segments& another, int relation){
	return *this;
}

CSGTree& CSGTree::Merge(const CSGTree& pB, TYPE_RELATION rel){
	CSGNode temp;
	temp.lchild = root;
	temp.rchild = pB.root;
	temp.isLeaf = false;
	temp.relation = rel;
	return *this;
}

SegmentPoint Segments::firstPositive(){
	vector<SegmentPoint>::iterator j;
	for (j = points.begin(); j != points.end(); j++){

	}
	return *j;
}

bool CSGTree::intersect(const ray& r, isect& i) const{
	Segments inters = intersectLocal(r);
	return true;
}

Segments CSGTree::intersectLocal(const ray& r) const{
	return Segments();
}

bool CSG::intersectLocal(const ray& r, isect& i) const
{
		return false;
}

BoundingBox CSG::ComputeLocalBoundingBox(){
	BoundingBox localbounds;
	localbounds.max = vec3f(0.5, 0.5, 0.5);
	localbounds.min = vec3f(-0.5, -0.5, -0.5);
	return localbounds;
}
