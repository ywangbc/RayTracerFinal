// The main ray tracer.

#include <Fl/fl_ask.h>

#include "RayTracer.h"
#include "scene/light.h"
#include "scene/material.h"
#include "scene/ray.h"
#include "fileio/read.h"
#include "fileio/parse.h"
#include "fileio/bitmap.h"

// Trace a top-level ray through normalized window coordinates (x,y)
// through the projection plane, and out into the scene.  All we do is
// enter the main ray-tracing method, getting things started by plugging
// in an initial ray weight of (0.0,0.0,0.0) and an initial recursion depth of 0.
vec3f RayTracer::trace( Scene *scene, double x, double y )
{
    ray r( vec3f(0,0,0), vec3f(0,0,0) );
    scene->getCamera()->rayThrough( x,y,r );
	mediaHistory.clear();

	return traceRay( scene, r, vec3f(1.0,1.0,1.0), 0 ).clamp();
}

// Do recursive ray tracing!  You'll want to insert a lot of code here
// (or places called from here) to handle reflection, refraction, etc etc.
vec3f RayTracer::traceRay( Scene *scene, const ray& r, 
	const vec3f& thresh, int depth )
{
	isect i;

	if( scene->intersect( r, i ) ) {
		// YOUR CODE HERE

		// An intersection occured!  We've got work to do.  For now,
		// this code gets the material for the surface that was intersected,
		// and asks that material to provide a color for the ray.  



		// This is a great place to insert code for recursive ray tracing.
		// Instead of just returning the result of shade(), add some
		// more steps: add in the contributions from reflected and refracted
		// rays.

		const Material& m = i.getMaterial();
		vec3f I = m.shade(scene, r, i);
		if (depth >= maxDepth)return I;

		vec3f conPoint = r.at(i.t);
		vec3f normal;
		vec3f Rdir = 2 * (i.N*-r.getDirection()) * i.N - (-r.getDirection());
		//reflection
		ray R = ray(conPoint, Rdir);
		if(!i.getMaterial().kr.iszero())I += prod(i.getMaterial().kr,traceRay(scene, R, thresh, depth + 1));

		//if not opaque
		if (!i.getMaterial().kt.iszero()){

			bool TIR = false;

			//refraction
			ray T(conPoint, r.getDirection());//without refraction

			bool toAdd = false, toErase = false;
			//if not surface
			if (i.obj->hasInterior()){
				//calculate angle
				//in or out
				double indexA, indexB;
				if (i.N*r.getDirection() > RAY_EPSILON){//out
					if (mediaHistory.empty())indexA = 1.0;
					else indexA = mediaHistory.rbegin()->second.index;

					mediaHistory.erase(i.obj->getOrder());
					toAdd = true;
					if (mediaHistory.empty())indexB = 1.0;
					else {
						indexB = mediaHistory.rbegin()->second.index;
					}
					normal = -i.N;
				}
				else {//in
					if (mediaHistory.empty())indexA = 1.0;
					else indexA = mediaHistory.rbegin()->second.index;
					
					mediaHistory.insert(make_pair(i.obj->getOrder(),i.getMaterial()));
					toErase = true;
					indexB = mediaHistory.rbegin()->second.index;
					normal = i.N;
				}
				double indexRatio = indexA / indexB;
				double cdi = normal*-r.getDirection();
				double sdi = 1 - cdi*cdi;
				double sdt = sdi * indexRatio; //sin delta t
				//TIR
				if (sdt > 1.0 + RAY_EPSILON){
					TIR = true;
				}
				else {
					TIR = false;
					double cdt = sqrt(1 - sdt*sdt);
					vec3f Tdir = (indexRatio*cdi - cdt)*normal - indexRatio*-r.getDirection();
					T = ray(conPoint, Tdir);
				}
			}
			if(!TIR)I += prod(i.getMaterial().kt, traceRay(scene, T, thresh, depth + 1));
			if (toAdd)mediaHistory.insert(make_pair(i.obj->getOrder(), i.getMaterial()));
			if (toErase)mediaHistory.erase(i.obj->getOrder());
		}
		I = I.clamp();

		return I;
	
	} else {
		// No intersection.  This ray travels to infinity, so we color
		// it according to the background color, which in this (simple) case
		// is just black.
		if (!useBackground)return vec3f(0.0, 0.0, 0.0);
		else {
			vec3f axis_x = scene->getCamera()->getU();
			vec3f S = r.getDirection();
			S -= (S*axis_x)*axis_x;
			vec3f axis_z = scene->getCamera()->getLook();
			vec3f sz = (S*axis_z) *axis_z;
			S -= sz;
			vec3f axis_v = scene->getCamera()->getV();
			vec3f axis_y = axis_x.cross(axis_z);
			double dis_v = (S*axis_y);
			S = r.getDirection() - dis_v * axis_v;
			double dis_x = S * axis_x;
			double dis_z = S * axis_z;
			vec3f res = dis_x * axis_x + dis_v * axis_v + dis_z * axis_z;
			return getBackgroundImage(dis_x / dis_z + 0.5, dis_v / dis_z + 0.5);
		}
		
	}
}

RayTracer::RayTracer():
mediaHistory(),backgroundImage(NULL), useBackground(false)
{
	buffer = NULL;
	buffer_width = buffer_height = 256;
	scene = NULL;
	maxDepth = 0;
	maxThresh = 1.0;

	m_bSceneLoaded = false;
}


RayTracer::~RayTracer()
{
	delete [] buffer;
	delete scene;
	if (backgroundImage)delete[] backgroundImage;
}

void RayTracer::getBuffer( unsigned char *&buf, int &w, int &h )
{
	buf = buffer;
	w = buffer_width;
	h = buffer_height;
}

double RayTracer::aspectRatio()
{
	return scene ? scene->getCamera()->getAspectRatio() : 1;
}

bool RayTracer::sceneLoaded()
{
	return m_bSceneLoaded;
}

bool RayTracer::loadScene( char* fn )
{
	try
	{
		scene = readScene( fn );
	}
	catch( ParseError pe )
	{
		fl_alert( "ParseError: %s\n", pe );
		return false;
	}

	if( !scene )
		return false;
	
	buffer_width = 256;
	buffer_height = (int)(buffer_width / scene->getCamera()->getAspectRatio() + 0.5);

	bufferSize = buffer_width * buffer_height * 3;
	buffer = new unsigned char[ bufferSize ];
	
	// separate objects into bounded and unbounded
	scene->initScene();
	
	// Add any specialized scene loading code here
	
	m_bSceneLoaded = true;

	return true;
}

void RayTracer::loadBackground(char* fn)
{
	unsigned char* data = NULL;
	data = readBMP(fn, m_bWidth, m_bHeight);
	if (data){
		if (backgroundImage)delete[] backgroundImage;
		useBackground = true;
		backgroundImage = data;
	}
}

void RayTracer::clearBackground(){
	if (backgroundImage)delete [] backgroundImage;
	backgroundImage = NULL;
	useBackground = false;
	m_bHeight = m_bWidth = 0;
}


vec3f RayTracer::getBackgroundImage(double x, double y){
	if (!useBackground)return vec3f(0, 0, 0);
	int xGrid, yGrid;
	xGrid = int(x*m_bWidth);
	yGrid = int(y*m_bHeight);
	if (xGrid<0 || xGrid>=m_bWidth || yGrid<0 || yGrid>=m_bHeight)return vec3f(0, 0, 0);
	return vec3f(backgroundImage[(yGrid*m_bWidth + xGrid) * 3]/255.0, backgroundImage[(yGrid*m_bWidth + xGrid) * 3 + 1]/255.0, backgroundImage[(yGrid*m_bWidth + xGrid) * 3 + 2]/255.0);
}


void RayTracer::traceSetup( int w, int h )
{
	if( buffer_width != w || buffer_height != h )
	{
		buffer_width = w;
		buffer_height = h;

		bufferSize = buffer_width * buffer_height * 3;
		delete [] buffer;
		buffer = new unsigned char[ bufferSize ];
	}
	memset( buffer, 0, w*h*3 );
}

void RayTracer::traceLines( int start, int stop )
{
	vec3f col;
	if( !scene )
		return;

	if( stop > buffer_height )
		stop = buffer_height;

	for( int j = start; j < stop; ++j )
		for( int i = 0; i < buffer_width; ++i )
			tracePixel(i,j);
}

void RayTracer::tracePixel( int i, int j )
{
	vec3f col;

	if( !scene )
		return;

	double x = double(i)/double(buffer_width);
	double y = double(j)/double(buffer_height);

	col = trace( scene,x,y );

	unsigned char *pixel = buffer + ( i + j * buffer_width ) * 3;

	pixel[0] = (int)( 255.0 * col[0]);
	pixel[1] = (int)( 255.0 * col[1]);
	pixel[2] = (int)( 255.0 * col[2]);
}