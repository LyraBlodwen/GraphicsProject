#include <cmath>
#include <cstdio>
#include "objects.h"
#include "kdtree.hpp"
#include "parameters.hpp"

double rand01()
{
	return ((double) rand() / (RAND_MAX));
}

int sphere_id;
std::pair<double, vec3d> raytrace(const vec3d &dir, const vec3d &pos)
{
	ray r(pos, dir);
	std::pair<double, vec3d> intersect = std::make_pair(OO, vec3d());
	sphere_id = kNumSphere;
	int i;
	for(i=0; i<kNumSphere; i++)
	{
		std::pair<double, vec3d> interse = spheres[i].intersect(r);
		if(interse.first < intersect.first && interse.first > 0.01)
		{
			intersect = interse;
			sphere_id = i;
		}
	}
	return intersect;
}

vec3d reflect(const vec3d &ray, const vec3d &pos)
{
	vec3d norm = spheres[sphere_id].normal(pos);
	return (ray - norm * (2 * ray.dot(norm))).normalize();
}

bool total_refl;

vec3d refract(const vec3d &ray, const vec3d &pos, double n1, double n2)
{
	vec3d norm = spheres[sphere_id].normal(pos);
	vec3d w = ray * -1;
	if (n2==1) norm = norm * -1;
	if (1-(n1/n2)*(n1/n2)*(1-w.dot(norm)*w.dot(norm)) < 0)
	{
		total_refl = true;
		return reflect(ray, pos);
	}
	else total_refl = false;
	vec3d refr = (w-norm*w.dot(norm))*(-n1/n2)-norm*sqrt(1-(n1/n2)*(n1/n2)*(1-w.dot(norm)*w.dot(norm)));
	return refr.normalize();
}

vec3d c[1024*1024];

void drawPhoton(const vec3d &p, const vec3d &rgb){
	if(p.z>170)return;
	int y = kImgW/2 + (p.x-50)/(170-p.z)*kImgW*0.8;
	int x = kImgH/2 - (p.y-40.8)/(170-p.z)*kImgH*0.8;
//	printf("%.3lf %.3lf %.3lf\n",p.x,p.y,p.z);
//	printf("%d %d\n",x,y);
//	printf("%d %d %d\n",int(rgb.x*255),int(rgb.y*255),int(rgb.z*255));
	if (x>=0 && x<kImgH && y < kImgW && y>=0)
	{
		c[int(x*kImgW+y)] = rgb * 255;
	}
}

void shadowPhoton(const vec3d &ray, std::pair<double, vec3d> &intersect){
	vec3d shadow(-0.25,-0.25,-0.25);
	vec3d bumpedPoint = intersect.second + ray * 0.00001;
	raytrace(ray, bumpedPoint);
	vec3d shadowPoint = ray * intersect.first + bumpedPoint;
	storePhoton(shadowPoint, ray, shadow);
//	drawPhoton(shadowPoint, shadow);
}


void findIntersect(vec3d &ray, std::pair<double, vec3d> &intersect, int cnt, bool refl_diff = false)
{
	if (cnt == 0) return;
	if (spheres[sphere_id].material == SPEC || (refl_diff && spheres[sphere_id].material == DIFF && rand01()<0.4))
	{
		ray = reflect(ray, intersect.second);
		intersect = raytrace(ray, intersect.second);
		findIntersect(ray, intersect, cnt-1);
	}
	else if (spheres[sphere_id].material == REFR)
	{
//		puts("$$$$");
		ray = refract(ray, intersect.second, 1, 1.5);
//		intersect.second.pr();
		intersect = raytrace(ray, intersect.second);
//		intersect.second.pr();
		ray = refract(ray, intersect.second, 1.5, 1);
		intersect = raytrace(ray, intersect.second);
//		intersect.second.pr();
		findIntersect(ray, intersect, cnt-1);
	}
	else if (refl_diff && spheres[sphere_id].material == DIFF)
	{
		double r1 = 2 * M_PI * rand01(), r2 = rand01(), r2s = sqrt(r2);
		vec3d w=spheres[sphere_id].normal(intersect.second), u=((fabs(w.x)>.1?vec3d(0,1,0):vec3d(1,0,0))*w).normalize(), v=w*u;
		ray = (u*cos(r1)*r2s + v*sin(r1)*r2s + w*sqrt(1-r2)).normalize();
		intersect = raytrace(ray, intersect.second);
		findIntersect(ray, intersect, cnt-1);
	}
	else return;
}

void emitPhotons(){
	srand(233);
	for (int i = 0; i < kPhotons; i++){
		int bounces = 1;
		vec3d ray(rand01()-0.5, -rand01()/2, rand01()-0.5);
		ray = ray.normalize();
		vec3d lightStart = Light;
		lightStart = lightStart + vec3d((rand01()-0.5)*20, 0, (rand01()-0.5)*20);
		if (i > kPhotons * 0.5)
		{
			ray = vec3d(rand01()-0.5, rand01()-0.5, rand01()-0.5).normalize();
			lightStart = vec3d(rand01()*100, rand01()*81.6, rand01()*170);
		}
		std::pair<double, vec3d> intersect = raytrace(ray, lightStart);
		vec3d rgb(1,1,1);
		
		while (intersect.first != OO && bounces <= kBounces)
		{
			vec3d color = spheres[sphere_id].color;
//			if (sphere_id == 4)
//			{
//				if (((int(intersect.second.x/20)&1)+(int(intersect.second.z/20)&1))&1) color = vec3d(0.1,0.1,0.1);
//			}
			rgb = rgb.mult(color);
//			rgb = rgb.mult(spheres[sphere_id].color) * (1.0 / sqrt(bounces));
//			printf("#%.3lf %.3lf %.3lf\n",rgb.x, rgb.y,rgb.z);
			if (spheres[sphere_id].material == DIFF)
			{
				rgb = rgb * 0.8;
				storePhoton(intersect.second, ray, rgb);
				drawPhoton(intersect.second, rgb);
				if(rand01()<0.2)break;
			}
//			shadowPhoton(ray, intersect);
			
			findIntersect(ray, intersect, 1, true);
			bounces++;
		}
	}
}


vec3d calculateColor(int x, int y)
{
	vec3d ray((y/kImgW - 0.5)/1, (x/kImgH - 0.5)/1, -0.9);
	ray=ray.normalize();
	std::pair<double, vec3d> intersect = raytrace(ray, vec3d(50,40,170));
	if (intersect.first != OO)
	{
		findIntersect(ray, intersect, 4);
		if(intersect.first != OO)
		{
			if (spheres[sphere_id].material == DIFF && spheres[sphere_id].color == vec3d()) return vec3d();
			vec3d color = getPhotonAround(intersect.second, spheres[sphere_id].normal(intersect.second));
			return color;
		}
	}
	return vec3d(0,0,0);
}

int main()
{
	emitPhotons();
	FILE *f0 = fopen("photon_map.ppm", "w");
	int w,h;
	w = kImgW;
	h = kImgH;
	fprintf(f0, "P3\n%d %d\n%d\n", w, h, 255);
	for (int i=0; i<w*h; i++)
	{
//		printf("%d %d %d\n", int(c[i].x), int(c[i].y), int(c[i].z));
		fprintf(f0,"%d %d %d ", int(c[i].x), int(c[i].y), int(c[i].z));
	}
	FILE *f = fopen("image.ppm", "w");
	fprintf(f, "P3\n%d %d\n%d\n", w, h, 255);
	for (int i=0; i<h; i++)
	for (int j=0; j<w; j++)
	{
		vec3d color = calculateColor(h-i, j);
//		if (color.x+color.y+color.z)printf("%d %d %d\n", int(color.x), int(color.y), int(color.z));
		fprintf(f,"%d %d %d ", int(color.x), int(color.y), int(color.z));
	}
	printf("%d\n",tot_points);
	return 0;
}
