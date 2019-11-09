#include "objects.h"

std::pair<double, vec3d> Sphere::intersect(const ray& r) const
{
	vec3d op = center-r.o;
	double t, b=op.dot(r.d), det=b*b-op.dot(op)+radius*radius;
	if (det<0) return std::make_pair(OO,vec3d());
	else det=sqrt(det);
	t = (t=b-det)>eps ? t : ((t=b+det)>eps ? t : 0);
	return std::make_pair(t,r.pos(t));
}

std::pair<double, vec3d> Plane::intersect(const ray& p) const
{
	double t = -(offset + norm.dot(p.o)) / (norm.dot(p.d));
	if (t < 0 || t > OO) return std::make_pair(OO, vec3d());
	return std::make_pair(t, p.pos(t));
}

vec3d Sphere::normal(const vec3d &p) const
{
	return (p - center).normalize();
}

vec3d Plane::normal(const vec3d &p) const
{
	return norm;
}