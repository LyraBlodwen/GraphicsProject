#pragma once

#include <cmath>
#include <cstdio>

struct vec3d
{
	double x, y, z;
	
	vec3d(): x(0), y(0), z(0) {}
	vec3d(double x_, double y_, double z_): x(x_), y(y_), z(z_) {}
	
	void operator=(const vec3d &p)
	{ x = p.x; y = p.y; z = p.z; }
	
	bool operator==(const vec3d &p) const
	{ return x==p.x && y==p.y && z==p.z; }
	
	bool operator!=(const vec3d &p) const
	{ return x!=p.x || y!=p.y || z!=p.z; }
	
	vec3d operator+(const vec3d &p) const
	{ return vec3d(x + p.x, y + p.y, z + p.z); }
	
	vec3d operator-(const vec3d &p) const
	{ return vec3d(x - p.x, y - p.y, z - p.z); }
	
	vec3d operator*(const double& w) const
	{ return vec3d(x * w, y * w, z * w); }
	
	vec3d operator/(const double& w) const
	{ return vec3d(x / w, y / w, z / w); }
	
	double dot(const vec3d &p) const
	{ return x * p.x + y * p.y + z * p.z; }
	
	vec3d operator*(const vec3d &p) const
	{ return vec3d(y * p.z - z * p.y, z * p.x - x * p.z, x * p.y - y * p.x); }
	
	vec3d max(const vec3d &p) const
	{ return vec3d(std::max(x, p.x), std::max(y, p.y), std::max(z, p.z)); }
	
	vec3d min(const vec3d &p) const
	{ return vec3d(std::min(x, p.x), std::min(y, p.y), std::min(z, p.z)); }
	
	vec3d mult(const vec3d &p) const
	{ return vec3d(x * p.x, y * p.y, z * p.z); }
	
	double dist(const vec3d &p) const
	{ return sqrt((x - p.x) * (x - p.x) + (y - p.y) * (y - p.y) + (z - p.z) * (z - p.z)); }
	
	double len() const
	{ return sqrt(x * x + y * y + z * z); }
	
	double lensqr() const
	{ return (x * x + y * y + z * z); }
	
	vec3d normalize() const
	{ return *this / len(); }
	
	void pr() const
	{ printf("vec3d: %.5lf %.5lf %.5lf\n", x, y, z); }
};

struct ray
{
	vec3d o, d;
	
	ray(vec3d o_, vec3d d_): o(o_), d(d_) {}
	vec3d pos(const double& t) const
	{ return o + d * t; }
	
	void pr()
	{ printf("ray:\no: "); o.pr(); printf("d: "); d.pr(); }
};
