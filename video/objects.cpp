#include "objects.h"

std::pair<double, vec3d> Sphere::intersect(const ray& r)
{
	vec3d op = center-r.o;
	double t, b=op.dot(r.d), det=b*b-op.dot(op)+radius*radius;
	if (det<0) return std::make_pair(OO,vec3d());
	else det=sqrt(det);
	t = (t=b-det)>eps ? t : ((t=b+det)>eps ? t : 0);
	return std::make_pair(t,r.pos(t));
}

std::pair<double, vec3d> Plane::intersect(const ray& p)
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

bool BezierRotate::onBezier(double t, vec3d p) const
{
	double x = p.dist(vec3d(0, p.y, 0));
	if (std::abs(x - bez.x(t)) > eps) return false;
	if (std::abs(p.y - bez.y(t)) > eps) return false;
	return true;
}

std::pair<double, vec3d> BezierRotate::intersect(const ray& r)
{
	std::pair<double, vec3d> ans = std::make_pair(OO, vec3d());
	vec3d o = (r.o - pos).rotate_z(theta);
	vec3d d = (r.d).rotate_z(theta);
	
	long double sqdis = o.x * (o+d).z - o.z * (o+d).x;
	if (sqdis > bez.max_rad) return ans;
	sqdis = sqdis*sqdis;
	
	bool flag_y0 = false;
	long double t;
	long double a1, a2, a3, a4;
	long double b1, b2, b3;
	if (std::abs(d.y) > eps)
	{
		a1 = d.x/d.y, a2 = d.z/d.y, a3 = (o.x - d.x * o.y / d.y), a4 = (o.z - d.z * o.y / d.y);
		b1 = a1*a1 + a2*a2, b2 = 2.*a1*a3 + 2.*a2*a4, b3 = a3*a3 + a4*a4;
	}
	else flag_y0 = true;
	
	for (double t0 = 0.05; t0 <= 1; t0 += 0.1)
	{
		t = t0;
		int cnt = 0;
		long double yt, dyt, xt, dxt;
		do {
			cnt++;
			yt = bez.y(t); dyt = bez.dy(t);
			long double f, f_;
			if (flag_y0)
			{
				f = yt - o.y;
				f_ = dyt;
			}
			else {
				xt = bez.x(t); dxt = bez.dx(t);
				f = b1*yt*yt + b2*yt + b3 - xt*xt;
				f_ = 2.*b1*yt*dyt + b2*dyt - 2.*xt*dxt;
			}
			t = t - f/f_;
		}while(cnt < 20);
		xt = bez.x(t); dxt = bez.dx(t);
		yt = bez.y(t); dyt = bez.dy(t);
		
		vec3d p;
		long double travel_time;
		if (flag_y0)
		{
			if (o.x*o.x + o.z*o.z - sqdis < 0 || xt*xt - sqdis < 0) continue;
			travel_time = sqrt(o.x*o.x + o.z*o.z - sqdis) - sqrt(xt*xt - sqdis);
		}
		else travel_time = (yt-o.y)/d.y;
		
		if (travel_time < 0.01) continue;
		
		p = o + d * travel_time;
		vec3d now_norm(dyt*p.x/sqrt(p.x*p.x + p.z*p.z), -dxt, dyt*p.z/sqrt(p.x*p.x + p.z*p.z));
		if (onBezier(t, p))
		{
			if (travel_time < ans.first)
			{
				ans = std::make_pair(travel_time, p.rotate_z(-theta) + pos);
				norm = now_norm.normalize() * -1;
			}
		}
	}
//	if (flag_y0)
//	{
//		printf("%.3lf\n",ans.first);
//		ans.second.pr();
//	}
	return ans;
}

vec3d BezierRotate::normal(const vec3d &p) const
{
	return norm;
}