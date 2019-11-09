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
	
//	virtual std::pair<double, vec3d> intersect(const ray &p) = 0;
	virtual vec3d normal(const vec3d &p) const = 0;
	
};

class Sphere: public Object
{
  public:
	Sphere(vec3d center_, double radius_,
		   Refl_t material_ = DIFF, vec3d emission_ = vec3d(), vec3d color_ = vec3d()): 
		center(center_), radius(radius_), material(material_), emission(emission_), color(color_) {}
	
	std::pair<double, vec3d> intersect(const ray &p) ;
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
	
	std::pair<double, vec3d> intersect(const ray &p) ;
	vec3d normal(const vec3d &p) const override;
	
  public:
	vec3d norm, pos;
	double offset;
	Refl_t material;
	vec3d emission;
	vec3d color;
};

struct bezier
{
	int n;
	double ax[20], ay[20];
	double max_rad = 0;
	bezier() {}
	bezier(const int &control_points, const double *bx, const double *by)
	{
		n = control_points;
		int c[20][20];
		memset(c, 0, sizeof(c));
		for(int i = 0; i <= n; i++)
		{
			max_rad = std::max(max_rad, by[i]);
			ax[i]=ay[i]=0;
			c[i][0] = 1;
			for(int j = 1; j <= i; j++)
				c[i][j] = c[i-1][j] + c[i-1][j-1];
		}
		for(int i = 0; i <= n; i++)
		{
			double flag = 1;
			for(int j = 0; j <= n-i; j++)
			{
				ax[i+j] += bx[i] * c[n][i] * flag * c[n-i][j];
				ay[i+j] += by[i] * c[n][i] * flag * c[n-i][j];
				flag *= -1;
			}
		}
	}
	
	double x(const double &t) const
	{
		if (t<0) return -t;
		if (t>1) return t;
		double wx = 0;
		for(int i = n; i >= 0; i--)
			wx = wx * t + ax[i];
		return wx;
	}
	
	double y(const double &t) const
	{
		if (t<0) return -t;
		if (t>1) return t;
		double wy = 0;
		for(int i = n; i >= 0; i--)
			wy = wy * t + ay[i];
		return wy;
	}
	
	double dx(const double &t) const
	{
		if (t<0) return -t;
		if (t>1) return t;
		double wdx = 0;
		for(int i = n; i > 0; i--)
			wdx = wdx * t + ax[i] * i;
		return wdx;
	}
	
	double dy(const double &t) const
	{
		if (t<0) return -t;
		if (t>1) return t;
		double wdy = 0;
		for(int i = n; i > 0; i--)
			wdy = wdy * t + ay[i] * i;
		return wdy;
	}
	
	void pr()
	{
		for(int i = 0; i <= n; i++)
		printf("%.2lf x^%d  ",ax[i],i);
		puts("");
		for(int i = 0; i <= n; i++)
		printf("%.2lf x^%d  ",ay[i],i);
		puts("");
	}
};

class BezierRotate: public Object
{
  public:
	BezierRotate(const bezier &bez_, vec3d pos_, double theta_, Refl_t material_, vec3d emission_ = vec3d(), vec3d color_ = vec3d()): 
		bez(bez_), pos(pos_), theta(theta_), material(material_), emission(emission_), color(color_) {}
	
	bool onBezier(double t, vec3d p) const;
	std::pair<double, vec3d> intersect(const ray &p) ;
	vec3d normal(const vec3d &p) const override;
	
	
  public:
	bezier bez;
	vec3d pos;
	double theta;
	vec3d norm;
	Refl_t material;
	vec3d emission;
	vec3d color;
};