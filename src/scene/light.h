#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "scene.h"

class Light
	: public SceneElement
{
public:
	virtual vec3f shadowAttenuation(const vec3f& P) const = 0;
	virtual double distanceAttenuation( const vec3f& P ) const = 0;
	virtual vec3f getColor( const vec3f& P ) const = 0;
	virtual vec3f getDirection( const vec3f& P ) const = 0;

protected:
	Light( Scene *scene, const vec3f& col )
		: SceneElement( scene ), color( col ) {}

	vec3f 		color;
};


class DirectionalLight
	: public Light
{
public:
	DirectionalLight( Scene *scene, const vec3f& orien, const vec3f& color )
		: Light( scene, color ), orientation( orien ) {}
	virtual vec3f shadowAttenuation(const vec3f& P) const;
	virtual double distanceAttenuation( const vec3f& P ) const;
	virtual vec3f getColor( const vec3f& P ) const;
	virtual vec3f getDirection( const vec3f& P ) const;

protected:
	vec3f 		orientation;
};

class PointLight
	: public Light
{
public:
	PointLight( Scene *scene, const vec3f& pos, const vec3f& color )
		: Light(scene, color), position(pos), const_atten_coeff(1.0), linear_atten_coeff(0.0), quadratic_atten_coeff(0.0) {}
	PointLight(Scene *scene, const vec3f& pos, const vec3f& color, const double cac, const double lac, const double qac)
		: Light(scene, color), position(pos), const_atten_coeff(cac), linear_atten_coeff(lac),quadratic_atten_coeff(qac) {}
	virtual vec3f shadowAttenuation(const vec3f& P) const;
	virtual double distanceAttenuation( const vec3f& P ) const;
	virtual vec3f getColor( const vec3f& P ) const;
	virtual vec3f getDirection( const vec3f& P ) const;

private:
	double const_atten_coeff, linear_atten_coeff, quadratic_atten_coeff;

protected:
	vec3f position;
};

class SpotLight
	: public Light
{
public:
	SpotLight(Scene *scene, const vec3f& pos, const vec3f& color)
		: Light(scene, color), position(pos), const_atten_coeff(1.0), linear_atten_coeff(0.0), quadratic_atten_coeff(0.0) {}
	SpotLight(Scene *scene, const vec3f& pos, const vec3f& color, const double cac, const double lac, const double qac)
		: Light(scene, color), position(pos), const_atten_coeff(cac), linear_atten_coeff(lac), quadratic_atten_coeff(qac) {}

	virtual vec3f shadowAttenuation(const vec3f& P) const;
	virtual double distanceAttenuation(const vec3f& P) const;
	virtual vec3f getColor(const vec3f& P) const;
	virtual vec3f getDirection(const vec3f& P) const;

private:
	double const_atten_coeff, linear_atten_coeff, quadratic_atten_coeff;

protected:
	vec3f position;

};


#endif // __LIGHT_H__
