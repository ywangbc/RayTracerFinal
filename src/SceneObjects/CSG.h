#ifndef __CSG_H__
#define __CSG_H__

#include "../scene/scene.h"

enum TYPE_RELATION{
	OR = 1,
	AND,
	MINUS
};

struct SegmentPoint{
	double t;
	vec3f normal;
	bool isRight;//0-left 1-right
	int contri;//contribution 1:union/intersect -1:minus
};

class Segments{
private:
	vector<SegmentPoint> points;
public:
	Segments& Merge(const Segments& another, int relation);
};

class CSGNode
{
public:
	CSGNode() :lchild( NULL ), rchild(NULL), item(NULL), relation(AND), isLeaf(0){};
	CSGNode *lchild, *rchild;
	Geometry* item;
	TYPE_RELATION relation;
	bool isLeaf;//isLeaf=false => item=NULL
};

class CSGTree{
public:
	CSGTree():root(NULL){}
	CSGTree(Geometry* obj, CSGNode* ptr){
		root = ptr;
		root->item = obj;
		root->isLeaf = true;
	}
	CSGTree& Merge(const CSGTree& pB, int relation);
	bool intersect(const ray& r, isect& i) const;
private:
	CSGNode* root;
	Segments& intersectLocal(const ray& r) const;
};

class CSG
	: public MaterialSceneObject
{
public:
	CSG(Scene *scene, Material *mat)
		: MaterialSceneObject(scene, mat)
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
