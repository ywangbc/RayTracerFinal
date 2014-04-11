#ifndef __CSG_H__
#define __CSG_H__

#include "../scene/scene.h"

enum TYPE_RELATION{
	OR = 1,
	AND,
	MINUS
};

class SegmentPoint{
public:
	double t;
	vec3f normal;
	bool isRight;//0-left 1-right
	int contri;//contribution 1:union/intersect -1:minus
	bool operator<(const SegmentPoint& other){
		return t < other.t - RAY_EPSILON;
	}
};

class Segments{
private:
	vector<SegmentPoint> points;
public:
	Segments() :
		points(){}
	Segments& Merge(const Segments& another, int relation);
	bool firstPositive(SegmentPoint& pt);
	void addPoint(SegmentPoint& pt){
		points.push_back(pt);
	}
};

class CSGNode
{
public:
	CSGNode() 
		:lchild( NULL ), rchild(NULL), item(NULL), relation(AND), isLeaf(0){
	}
	CSGNode *lchild, *rchild;
	Geometry* item;
	TYPE_RELATION relation;
	bool isLeaf;//isLeaf=false => item=NULL
	Segments intersectLocal(const ray& r) const;
	BoundingBox getBoundingBox() const;
	void computerBoundingBox();
private:
	BoundingBox bound;
};

class CSGTree{
public:
	CSGTree():root(NULL){}
	CSGTree(CSGNode* ptr){
		root = ptr;
	}
	CSGTree(const CSGTree& other){
		root = other.root;
	}
	CSGTree& Merge(const CSGTree& pB, TYPE_RELATION relation);
	bool intersect(const ray& r, isect& i) const;
	CSGNode* getRoot(){ return root; }
private:
	CSGNode* root;
};

class CSG
	: public MaterialSceneObject
{
public:
	CSG(Scene *scene, Material *mat, CSGTree tr)
		: MaterialSceneObject(scene, mat), tree(tr)
	{
	}
	virtual bool intersectLocal(const ray& r, isect& i) const;
	virtual bool hasBoundingBoxCapability() const { return true; }
	virtual bool hasInterior() const{ return true; }
	virtual BoundingBox ComputeLocalBoundingBox();
private:
	CSGTree tree;
};

#endif // __CSG_H__
