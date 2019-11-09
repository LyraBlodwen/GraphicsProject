#include <cstdio>
#include <vector>
#include <cstring>
#include <iostream>
#include <algorithm>
#include "objects.h"
#include "kdtree.hpp"
const int N=100010;
typedef long long ll;

void iWork()
{
	vec3d p(1,1,1);
	Sphere s(p, 1);
	ray r(vec3d(0,0,0), vec3d(1,1,1).normalize());
	std::pair<double, vec3d> ans;
	ans = s.intersect(r);
	printf("%.5lf\n",ans.first);
	ans.second.pr();
	s.normal(vec3d(1,1,0)).pr();
	
	Plane pl(vec3d(1,1,1),0);
	ans = pl.intersect(ray(vec3d(1,1,1), vec3d(-1,0,-1)));
	printf("%.5lf\n",ans.first);
	ans.second.pr();
	
	Plane pl2(vec3d(1,0,0),vec3d(1,1,1));
	ans = pl2.intersect(ray(vec3d(2,2,2), vec3d(-1,0,0)));
	printf("%.5lf\n",ans.first);
	ans.second.pr();
	
	
	vec3d position(0,0,0);
	vec3d direction(-1,-1,-1);
	vec3d color(1,1,1);
	vec3d norm(1,1,1);
	storePhoton(position, direction, color);
	storePhoton(position, direction, color);
	color = getPhotonAround(position, norm);
	color.pr();
}

int main()
{
	iWork();
	return 0;
}