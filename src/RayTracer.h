#ifndef __RAYTRACER_H__
#define __RAYTRACER_H__

// The main ray tracer.

#include "scene/scene.h"
#include "scene/ray.h"
#include <map>

class RayTracer
{
public:
    RayTracer();
    ~RayTracer();

    vec3f trace( Scene *scene, double x, double y );
	vec3f traceRay( Scene *scene, const ray& r, const vec3f& thresh, int depth );


	void getBuffer( unsigned char *&buf, int &w, int &h );
	double aspectRatio();
	void traceSetup( int w, int h );
	void traceLines( int start = 0, int stop = 10000000 );
	void tracePixel( int i, int j );
	void setDepth(int dep){
		maxDepth = dep;
	}
	bool loadScene( char* fn );
	void loadBackground( char* fn );
	vec3f getBackgroundImage(double x, double y);
	void clearBackground();
	bool sceneLoaded();

private:
	unsigned char *buffer;
	int buffer_width, buffer_height;
	int bufferSize;
	Scene *scene;
	int maxDepth;
	double maxThresh;
	bool useBackground;
	unsigned char *backgroundImage;
	int m_bHeight, m_bWidth;

	bool m_bSceneLoaded;

	std::map<int,Material> mediaHistory;
};

#endif // __RAYTRACER_H__
