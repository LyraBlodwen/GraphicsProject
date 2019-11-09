#include <cstdio>
#include <vector>
#include <cstring>
#include <iostream>
#include <algorithm>
#include "objects.h"
#include "matrix.hpp"
const int N=100010;
typedef long long ll;
using namespace std;

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
	
	matrix m;
	m.r=m.c=3;
	m.w[1][1]=13;m.w[1][2]=2;m.w[1][3]=3;
	m.w[2][1]=2;m.w[2][2]=5;m.w[2][3]=7;
	m.w[3][1]=7;m.w[3][2]=11;m.w[3][3]=13;
	m.inverse();
	m.pr("inverse");
	
	int n = 3;
	double bx[]={0,2,3,4};
	double by[]={8,11,13,15};
	bezier bez(n,bx,by);
	bez.pr();
	printf("%.3lf\n",bez.x(0.5));
	printf("%.3lf\n",bez.y(0.5));
	printf("%.3lf\n",bez.dx(0.5));
	printf("%.3lf\n",bez.dy(0.5));
//	printf("%.3lf\n",bez.ddx(2));
//	printf("%.3lf\n",bez.ddy(2));
	
	int bez_n = 3;
	double bx_[]={0,10,10,0};
	double by_[]={40,30,10,0};
	bezier beziercurve(bez_n,bx_,by_);
	BezierRotate BezObj(beziercurve, vec3d(0, 0, 0), 0, SPEC);
	cout<<BezObj.onBezier(0.5, vec3d(7.5, 20, 0))<<endl;
	std::pair<double, vec3d> bez_inter = BezObj.intersect(ray(vec3d(7.5+1,21,2), vec3d(-1,-1,-1).normalize()));
	printf("%.3lf\n",bez_inter.first);
	bez_inter.second.pr();
	BezObj.norm.pr();
	bez_inter = BezObj.intersect(ray(vec3d(7.5+100,20,7.5+100), vec3d(-1,0,-1).normalize()));
	printf("%.3lf\n",bez_inter.first);
	bez_inter.second.pr();
	BezObj.norm.pr();
}

int main()
{
	iWork();
	return 0;
}
/*

0.73205
vec3d: 0.42265 0.42265 0.42265
vec3d: 0.00000 0.00000 -1.00000
1.50000
vec3d: -0.50000 1.00000 -0.50000
1.00000
vec3d: 1.00000 2.00000 2.00000
inverse:
-------------------
	0.08054  -0.04698   0.00671
  -0.15436  -0.99329   0.57047
	0.08725   0.86577  -0.40940
-------------------
0.00 x^0  6.00 x^1  -3.00 x^2  1.00 x^3  
8.00 x^0  9.00 x^1  -3.00 x^2  1.00 x^3  
2.375
11.875
3.750
6.750
1
1.835
vec3d: 7.44072 19.94072 0.94072
vec3d: 0.99210 -0.00176 0.12543
144.528
vec3d: 5.30330 20.00000 5.30330
vec3d: 0.70711 0.00000 0.70711

*/