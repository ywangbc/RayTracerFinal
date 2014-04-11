#include <cmath>
#include <assert.h>
#include <cfloat>



#include "CSG.h"

Segments& Segments::Merge(const Segments& another, int relation){
	vector<SegmentPoint> interval;
	vector<SegmentPoint>::const_iterator j;
	SegmentPoint temp;
	for (j = points.begin(); j != points.end(); j++){
		temp = *j;
		temp.contri = 1;
		interval.push_back(temp);
	}
	for (j = another.points.begin(); j != another.points.end(); j++){
		temp = *j;
		if (relation == MINUS)temp.contri = -1;
		else temp.contri = 1;
		interval.push_back(temp);
	}
	int before=0, after=0;
	int require;
	if (relation == AND)require = 2;
	else require = 1;
	sort(interval.begin(), interval.end());
	points.clear();
	for (j = interval.begin(); j != interval.end(); j++){
		if(j->isRight)after -= j->contri;
		else after += j->contri;
		if (before < require&&after >= require){//enable
			temp = *j;
			temp.isRight = false;
			points.push_back(temp);
		}
		else if (before >= require&&after < require){//disable
			temp = *j;
			temp.isRight = true;
			points.push_back(temp);
		}
		before = after;
	}

	return *this;
}

CSGTree& CSGTree::Merge(const CSGTree& pB, TYPE_RELATION rel){
	CSGNode *temp;
	temp = new CSGNode;
	temp->lchild = root;
	temp->rchild = pB.root;
	temp->isLeaf = false;
	temp->relation = rel;
	temp->computerBoundingBox();
	root = temp;
	return *this;
}

bool Segments::firstPositive(SegmentPoint& pt){
	vector<SegmentPoint>::iterator j;
	bool hasOne = false;
	for (j = points.begin(); j != points.end(); j++){
		if (j->t > RAY_EPSILON){
			if (hasOne){
				if (j->t < pt.t)pt = *j;
			}
			else {
				hasOne = true;
				pt = *j;
			}
		}
	}
	return hasOne;
}

bool CSGTree::intersect(const ray& r, isect& i) const{
	if (!root)return false;
	Segments inters = root->intersectLocal(r);
	SegmentPoint sp;
	if(!inters.firstPositive(sp))return false;
	i.t = sp.t;
	if (sp.isRight){//right - out
		if (sp.normal*r.getDirection() > RAY_EPSILON)i.N = sp.normal;
		else i.N = -sp.normal;
	}
	else {//left - in
		if (sp.normal*r.getDirection() > RAY_EPSILON)i.N = -sp.normal;
		else i.N = sp.normal;
	}
	return true;
}

Segments CSGNode::intersectLocal(const ray& r) const{
	Segments ret;
	if (isLeaf){
		SegmentPoint pNear, pFar;
		isect i;
		ray backR(r.at(-10000), r.getDirection());
		if(!item->intersect(backR, i))return ret;
		pNear.t = i.t - 10000;
		pNear.normal = i.N;
		pNear.isRight = false;
		ray contiR(r.at(pNear.t+RAY_EPSILON*10),r.getDirection());
		if (!item->intersect(contiR, i))pFar = pNear;
		else {
			pFar.t = i.t + pNear.t;
			pFar.normal = i.N;
		}
		pFar.isRight = true;
		ret.addPoint(pNear);
		ret.addPoint(pFar);
		return ret;
	}
	else {
		if (!lchild || !rchild)return ret;
		Segments leftSeg, rightSeg;
		leftSeg = lchild->intersectLocal(r);
		rightSeg = rchild->intersectLocal(r);
		leftSeg.Merge(rightSeg,relation);
		return leftSeg;
	}
}

BoundingBox CSGNode::getBoundingBox() const{
	return bound;
}

void CSGNode::computerBoundingBox(){
	if (isLeaf)bound = item->getBoundingBox();
	else {
		if (!lchild || !rchild)bound = BoundingBox();
		bound = lchild->getBoundingBox();
		bound.plus(rchild->getBoundingBox());
	}
}

bool CSG::intersectLocal(const ray& r, isect& i) const
{
	if(!tree.intersect(r, i))return false;
	i.obj = this;
	return true;
}

BoundingBox CSG::ComputeLocalBoundingBox(){
	CSGNode* rt = tree.getRoot();
	if (!rt)return BoundingBox();
	return rt->getBoundingBox();
}
