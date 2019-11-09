#pragma once

#include <cstdio>
#include <algorithm>
#include "vec.hpp"

const double OO = 1e20;
const double eps = 1e-6;

enum Refl_t { DIFF, SPEC, REFR };

class Object
{
  public:
	Object() {}
	~Object() {}
	
	virtual std::pair<double, vec3d> intersect(const ray &p) const = 0;
	virtual vec3d normal(const vec3d &p) const = 0;
	
  public:
	Refl_t material;
	vec3d emission;
	vec3d color;
};

class Sphere: public Object
{
  public:
	Sphere(vec3d center_, double radius_,
		   Refl_t material_ = DIFF, vec3d emission_ = vec3d(), vec3d color_ = vec3d()): 
		center(center_), radius(radius_), material(material_), emission(emission_), color(color_) {}
	
	std::pair<double, vec3d> intersect(const ray &p) const override;
	vec3d normal(const vec3d &p) const override;
	
  public:
	vec3d center;
	double radius;
	Refl_t material;
	vec3d emission;
	vec3d color;
};

class Plane: public Object
{
  public:
	Plane(vec3d norm_, double offset_,
		  Refl_t material_ = DIFF, vec3d emission_ = vec3d(), vec3d color_ = vec3d()): 
		norm(norm_), offset(offset_),
		material(material_), emission(emission_), color(color_) {}
	Plane(vec3d norm_, vec3d pos_,
		  Refl_t material_ = DIFF, vec3d emission_ = vec3d(), vec3d color_ = vec3d()): 
		norm(norm_), pos(pos_),
		material(material_), emission(emission_), color(color_)
	{ offset = -norm.dot(pos); }
	
	std::pair<double, vec3d> intersect(const ray &p) const override;
	vec3d normal(const vec3d &p) const override;
	
  public:
	vec3d norm, pos;
	double offset;
	Refl_t material;
	vec3d emission;
	vec3d color;
};
