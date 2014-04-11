#include <cmath>
#include <assert.h>
#include <cfloat>



#include "CSG.h"

Segments& Segments::Merge(const Segments& another, int relation){
	return *this;
}

CSGTree& CSGTree::Merge(const CSGTree& pB, int relation){
	return *this;
}

bool CSGTree::intersect(const ray& r, isect& i) const{
	return false;
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
