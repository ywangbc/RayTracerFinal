#include "ray.h"
#include "material.h"
#include "light.h"

// Apply the phong model to this point on the surface of the object, returning
// the color of that point.
vec3f Material::shade( Scene *scene, const ray& r, const isect& i ) const
{
	// YOUR CODE HERE

	// For now, this method just returns the diffuse color of the object.
	// This gives a single matte color for every distinct surface in the
	// scene, and that's it.  Simple, but enough to get you started.
	// (It's also inconsistent with the phong model...)

	// Your mission is to fill in this method with the rest of the phong
	// shading model, including the contributions of all the light sources.
    // You will need to call both distanceAttenuation() and shadowAttenuation()
    // somewhere in your code in order to compute shadows and light falloff.

	vec3f ret;

	ret = ke + prod(vec3f(1,1,1)-kt,prod(ka,scene->getAmbient()));

	vec3f P;
	P = r.at(i.t);

	for (Scene::cliter j = scene->beginLights(); j != scene->endLights(); j++){
		vec3f atten = (*j)->distanceAttenuation(P)*(*j)->shadowAttenuation(P);
		vec3f V = (*j)->getDirection(P);
		vec3f diffuse_term = prod(kd*maximum(i.N*V,0),(vec3f(1,1,1)-kt));
		vec3f R = (2 * (i.N*V)*i.N) - V;
		R = R.normalize();
		vec3f specular_term = ks*(pow(maximum(R*-r.getDirection(),0), shininess*128.0));

		ret += prod(atten, diffuse_term + specular_term);
	}

	ret = ret.clamp();

	return ret;

}

vec3f Material::another_shade(Scene *scene, const ray& r, const isect& i) const
{
	// YOUR CODE HERE

	// For now, this method just returns the diffuse color of the object.
	// This gives a single matte color for every distinct surface in the
	// scene, and that's it.  Simple, but enough to get you started.
	// (It's also inconsistent with the phong model...)

	// Your mission is to fill in this method with the rest of the phong
	// shading model, including the contributions of all the light sources.
	// You will need to call both distanceAttenuation() and shadowAttenuation()
	// somewhere in your code in order to compute shadows and light falloff.

	vec3f ret;

	ret = ke + prod(vec3f(1, 1, 1) - kt, prod(ka, scene->getAmbient()));

	vec3f P;
	P = r.at(i.t);

	for (Scene::cliter j = scene->beginLights(); j != scene->endLights(); j++){
		vec3f atten = (*j)->distanceAttenuation(P)*(*j)->shadowAttenuation(P);
		vec3f V = (*j)->getDirection(P);
		vec3f diffuse_term = prod(kd*maximum(i.N*V, 0), (vec3f(1, 1, 1) - kt));
		vec3f R = (2 * (i.N*V)*i.N) - V;
		R = R.normalize();
		vec3f specular_term = ks*(pow(maximum(R*-r.getDirection(), 0), shininess*128.0));

		ret += prod(atten, diffuse_term + specular_term);
	}

	ret = ret.clamp();

	for (int i = 0; i < 3; i++){
		ret[i] = int(ret[i] * 10.0) / 10.0;
	}

	return ret;

}
