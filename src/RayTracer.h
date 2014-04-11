#ifndef __RAYTRACER_H__
#define __RAYTRACER_H__

// The main ray tracer.

#include "scene/scene.h"
#include "scene/ray.h"
#include <map>

const unsigned char ADPTHRE = 100;
const int SUBDEPTH = 3;
const double ADPTHREDOUBLE = ADPTHRE / 255.0;

class RayTracer
{
public:
    RayTracer();
    ~RayTracer();

    vec3f trace( Scene *scene, double x, double y );
	vec3f traceRay( Scene *scene, const ray& r, const vec3f& thresh, int depth );
	vec3f qSubPix(double xMin, double yMin, double width, double height, int depth);


	void getBuffer( unsigned char *&buf, int &w, int &h );
	double aspectRatio();
	void traceSetup( int w, int h );
	void traceLines( int start = 0, int stop = 10000000 );
	void tracePixel( int i, int j );
	void tracePixelSample(int i, int j);
	void tracePixelAdapt(int i, int j);
	void setDepth(int dep){
		maxDepth = dep;
	}
	void setThre(double threshold)
	{
		maxThresh = threshold;
	}
	void setAnti(int anti)
	{
		maxAnti = anti;
	}

	bool eq(double a, double b)
	{
		if (abs(a - b) < RAY_EPSILON) return true;
		else return false;
	}
	bool loadScene( char* fn );
	void loadBackground( char* fn );
	vec3f getBackgroundImage(double x, double y);
	void clearBackground();
	bool sceneLoaded();

private:
	unsigned char *buffer;
	//Trace the value of the result from corner
	unsigned char *cornerBuffer;
	// if the corner buffer is already filled
	bool* bufferFilled;

	int buffer_width, buffer_height;
	int bufferSize;
	int cornerBufferSize;
	int bufferFilledSize;
	Scene *scene;
	int maxDepth;
	double maxThresh;
	int maxAnti;
	bool useBackground;
	unsigned char *backgroundImage;
	int m_bHeight, m_bWidth;

	bool m_bSceneLoaded;

	std::map<int,Material> mediaHistory;
};

#endif // __RAYTRACER_H__
