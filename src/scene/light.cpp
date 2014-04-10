#include <cmath>

#include "light.h"

double DirectionalLight::distanceAttenuation( const vec3f& P ) const
{
	// distance to light is infinite, so f(di) goes to 0.  Return 1.
	return 1.0;
}


vec3f DirectionalLight::shadowAttenuation( const vec3f& P ) const
{
    // YOUR CODE HERE:
    // You should implement shadow-handling code here.
	vec3f d = getDirection(P);
	vec3f curP = P;
	isect isecP;
	vec3f ret = getColor(P);
	ray r = ray(curP, d);
	while (scene->intersect(r, isecP)){
		if (isecP.getMaterial().kt.iszero())return vec3f(0, 0, 0);
		ret = prod(ret, isecP.getMaterial().kt);
		curP = r.at(isecP.t);
		r = ray(curP, d);
	}

	return ret;
}

vec3f DirectionalLight::getColor( const vec3f& P ) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f DirectionalLight::getDirection( const vec3f& P ) const
{
	return -orientation;
}

double PointLight::distanceAttenuation( const vec3f& P ) const
{
	// YOUR CODE HERE
	// You'll need to modify this method to attenuate the intensity 
	// of the light based on the distance between the source and the 
	// point P.  For now, I assume no attenuation and just return 1.0

	double d = (position - P).length();
	
	double dA = 1.0/(d*d*quadratic_atten_coeff+d*linear_atten_coeff+const_atten_coeff);
	if (dA > 1.0) dA = 1.0;
	return dA;
}

vec3f PointLight::getColor( const vec3f& P ) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f PointLight::getDirection( const vec3f& P ) const
{
	return (position - P).normalize();
}


vec3f PointLight::shadowAttenuation(const vec3f& P) const
{
    // YOUR CODE HERE:
    // You should implement shadow-handling code here.
	vec3f d = getDirection(P);
	vec3f curP = P;
	isect isecP;
	vec3f ret = getColor(P);
	ray r = ray(curP, d);
	while (scene->intersect(r, isecP)){
		if (isecP.getMaterial().kt.iszero())return vec3f(0, 0, 0);
		ret = prod(ret, isecP.getMaterial().kt);
		curP = r.at(isecP.t);
		r = ray(curP, d);
	}

	return ret;

}

double SpotLight::distanceAttenuation(const vec3f& P) const
{
	// YOUR CODE HERE
	// You'll need to modify this method to attenuate the intensity 
	// of the light based on the distance between the source and the 
	// point P.  For now, I assume no attenuation and just return 1.0

	double d = (position - P).length();

	double dA = 1.0 / (d*d*quadratic_atten_coeff + d*linear_atten_coeff + const_atten_coeff);
	if (dA > 1.0) dA = 1.0;
	return dA;
}

vec3f SpotLight::getColor(const vec3f& P) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f SpotLight::getDirection(const vec3f& P) const
{
	return (position - P).normalize();
}


vec3f SpotLight::shadowAttenuation(const vec3f& P) const
{
	// YOUR CODE HERE:
	// You should implement shadow-handling code here.
	vec3f d = getDirection(P);
	vec3f curP = P;
	isect isecP;
	vec3f ret = getColor(P);
	ray r = ray(curP, d);
	while (scene->intersect(r, isecP)){
		if (isecP.getMaterial().kt.iszero())return vec3f(0, 0, 0);
		ret = prod(ret, isecP.getMaterial().kt);
		curP = r.at(isecP.t);
		r = ray(curP, d);
	}

	double cosVal = -d.normalize()*castDir.normalize();
	if (cosVal < 0)
	{
		cosVal = 0;
	}

	double strength = pow(cosVal, range);
	vec3f effect = vec3f(strength, strength, strength);

	ret = prod(ret, effect);

	return ret;

}