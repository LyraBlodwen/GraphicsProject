#include <cmath>
#include <cstdio>
#include "objects.h"
#include "kdtree.hpp"
#include "parameters.hpp"

int image_id;
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
	std::pair<double, vec3d> interse = BezObj.intersect(r);
	if(interse.first < intersect.first && interse.first > 0.01)
	{
		intersect = interse;
		sphere_id = -1;
	}
	return intersect;
}

vec3d reflect(const vec3d &ray, const vec3d &pos)
{
	vec3d norm;
	if (sphere_id == -1) norm = BezObj.norm;
	else norm = spheres[sphere_id].normal(pos);
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

int bg1_w, bg1_h, bg2_w, bg2_h;
vec3d colormap[2024*2024], bg2[1130*2500];
double distmap[2024*2024];
//vec3d bg1[2600*2600];
//double diff_k;

void findIntersect(vec3d &ray, std::pair<double, vec3d> &intersect, int cnt, bool refl_diff = false)
{
//	diff_k = 1.0;
	if (cnt == 0) return;
	if (sphere_id == -1 || spheres[sphere_id].material == SPEC || (refl_diff && spheres[sphere_id].material == REFR && rand01()<0.2) || (refl_diff && spheres[sphere_id].material == DIFF && rand01()<0.4))
	{
		ray = reflect(ray, intersect.second);
		intersect = raytrace(ray, intersect.second);
		findIntersect(ray, intersect, cnt-1);
	}
	else if (spheres[sphere_id].material == REFR)
	{
//		puts("$$$$");
		ray = refract(ray, intersect.second, 1, 1.33);
//		intersect.second.pr();
		intersect = raytrace(ray, intersect.second);
//		intersect.second.pr();
		ray = refract(ray, intersect.second, 1.33, 1);
		intersect = raytrace(ray, intersect.second);
//		intersect.second.pr();
		findIntersect(ray, intersect, cnt-1);
	}
	else if (refl_diff && spheres[sphere_id].material == DIFF)
	{
//		vec3d refl_ray = reflect(ray, intersect.second);
//		vec3d ori_ray = ray;
		double r1 = 2 * M_PI * rand01(), r2 = rand01(), r2s = sqrt(r2);
		vec3d w=spheres[sphere_id].normal(intersect.second), u=((fabs(w.x)>.1?vec3d(0,1,0):vec3d(1,0,0))*w).normalize(), v=w*u;
		ray = (u*cos(r1)*r2s + v*sin(r1)*r2s + w*sqrt(1-r2)).normalize();
//		diff_k = 0.2 + 0.8 * std::pow(std::abs(refl_ray.dot(ray)), 5)/std::abs(w.dot(ori_ray));
		intersect = raytrace(ray, intersect.second);
		findIntersect(ray, intersect, cnt-1);
	}
	else return;
}

vec3d getFinalColor(const vec3d &color)
{
	return (color * 255 * kExposure).min(vec3d(255,255,255));
}

vec3d getBlurredColor(int x, int y)
{
	vec3d color;
	double cnt = 0;
	int window_size = std::abs(kFocusDist-distmap[int(x*kImgW+y)])/kAparture;
	for(int i=std::max(0, x-window_size); i<=std::min(int(kImgH-1),x+window_size); i++)
	{
		for(int j=std::max(0, y-window_size); j<=std::min(int(kImgW-1),y+window_size); j++)
		{
			color = color + getFinalColor(colormap[int(i*kImgW+j)]);
			cnt += 1;
		}
	}
	return color / cnt;
}

void printColorMap()
{
	int h = kImgH/2, w = kImgW/2;
	std::string file_name = "image";
	file_name = file_name + char(int(image_id/10) + '0'); 
	file_name = file_name + char(int(image_id%10) + '0');
	file_name = file_name + ".ppm";
	FILE *f = fopen(file_name.c_str(), "w");
	fprintf(f, "P3\n%d %d\n%d\n", w, h, 255);
	for (int i=h-1; i>=0; i--)
	for (int j=0; j<w; j++)
	{
		vec3d color = getBlurredColor(i*2, j*2) + getBlurredColor(i*2, j*2+1) + getBlurredColor(i*2+1, j*2) + getBlurredColor(i*2+1, j*2+1);
		color = color / 4.0;
		fprintf(f,"%d %d %d ", int(color.x), int(color.y), int(color.z));
	}
	fclose(f);
}

void emitPhotons(){
	srand(233);
	double radius = kRadius;
	double total_modify = 0;
	for (int i = 0; i < kPhotons; i++){
		int bounces = 1;
		vec3d ray(rand01()-0.5, rand01()-0.5, rand01()-0.5);
		ray = ray.normalize();
		vec3d rgb(1.,1.,1.);
		vec3d lightStart = Light;
		lightStart = lightStart + vec3d((rand01()-0.5), (rand01()-0.5), (rand01()-0.5)).normalize() * 10.5;
		if (rand01()<0.2)
		{
			lightStart = vec3d(rand01()*90+5, rand01()*70+5, rand01()*160+5);
		}
		std::pair<double, vec3d> intersect = raytrace(ray, lightStart);
		
		while (intersect.first != OO && intersect.second.min(vec3d(140, 130, 170)) == intersect.second && intersect.second.max(vec3d()) == intersect.second && bounces <= kBounces)
		{
			vec3d color;
			if (sphere_id != -1) color = spheres[sphere_id].color;
			else color = vec3d(1,1,1);
			if (sphere_id == 4 || sphere_id == 5)
			{
				int bg_x, bg_y;
				bg_x = intersect.second.x/140*bg2_h;
				bg_y = intersect.second.z/170*bg2_w;
				if (bg_x>=0 && bg_x<bg2_h && bg_y>=0 && bg_y<= bg2_w) color = bg2[bg_x*bg2_w+bg_y]/255.0;
				else puts("???");
			}
			if (sphere_id == 2 || sphere_id == 3)
			{
				int bg_x, bg_y;
				bg_x = intersect.second.x/140*bg2_h;
				bg_y = intersect.second.y/130*bg2_w;
				if (bg_x>=0 && bg_x<bg2_h && bg_y>=0 && bg_y<= bg2_w) color = bg2[bg_x*bg2_w+bg_y]/255.0;
				else puts("???");
			}
			if (sphere_id == 0)
			{
				int bg_x, bg_y;
				bg_x = intersect.second.z/170*bg2_h;
				bg_y = bg2_w-intersect.second.y/130*bg2_w;
				if (bg_x>=0 && bg_x<bg2_h && bg_y>=0 && bg_y<= bg2_w) color = bg2[bg_x*bg2_w+bg_y]/255.0;
				else puts("???");
			}
			if (sphere_id == 1)
			{
				int bg_x, bg_y;
				bg_x = bg2_h-intersect.second.z/170*bg2_h;
				bg_y = intersect.second.y/130*bg2_w;
				if (bg_x>=0 && bg_x<bg2_h && bg_y>=0 && bg_y<= bg2_w) color = bg2[bg_x*bg2_w+bg_y]/255.0;
				else puts("???");
			}
			if (sphere_id == 9 && intersect.second.y <= 38)
			{
				if (intersect.second.dist(vec3d(50,38,52))<=3)
				{
					if (intersect.second.y >= 37) color = vec3d(.75,.25,.25);
					else color = vec3d(.75,.35,.35);
				}
			}
			rgb = rgb.mult(color);
			if (sphere_id != -1 && (spheres[sphere_id].material == DIFF || (spheres[sphere_id].material == REFR && rand01() < 0.2)))
			{
//				rgb = rgb * 0.8;
				vec3d norm = spheres[sphere_id].normal(intersect.second);
				int modify = updatePhotonAround(intersect.second, norm, rgb, colormap, radius);
				total_modify += modify;
				if(rand01()<0.2)break;
			}
			findIntersect(ray, intersect, 1, true);
//			rgb = rgb * diff_k;
			bounces++;
		}
		
		if (i % 100000 == 0)
		{
			total_modify /= 100000;
			printf("%.5lf   ",total_modify);
			total_modify = 0;
		}
		
		if (i % kStepPhotons == 0)
		{
			printf("\n# %d %.5lf\n", i/kStepPhotons, radius);
			printColorMap();
			if (radius > 0.02 && i % (kStepPhotons * 5) == 0) radius *= 0.9;
		}
	}
}


void calculateColor(int x, int y)
{
	vec3d ray((y/kImgW - 0.5)/1, (x/kImgH - 0.5)/1, -kFocusLength);
	ray=ray.normalize();
//	if (ray.y == 0) ray.pr();
	std::pair<double, vec3d> intersect = raytrace(ray, vec3d(50,40,170));
	if (intersect.first != OO)
	{
		distmap[int(x*kImgW+y)] = intersect.second.dist(vec3d(50,40,170));
		findIntersect(ray, intersect, 4);
		if(intersect.first != OO)
		{
			if (sphere_id == -1) storePhoton(x, y, intersect.second, BezObj.norm);
			else storePhoton(x, y, intersect.second, spheres[sphere_id].normal(intersect.second));
		}
	}
}

void readbg(std::string filename, vec3d *bg, int &bg_w, int &bg_h)
{
	FILE *f = fopen(filename.c_str(), "r");
	int t;
	fscanf(f, "P3\n%d %d\n%d\n", &bg_w, &bg_h,&t);
	printf("%d %d %d\n", bg_w, bg_h, t);
	for (int i=bg_h-1; i>=0; i--)
	for (int j=0; j<bg_w; j++)
	{
		int x, y, z;
		fscanf(f,"%d %d %d ", &x, &y, &z);
		bg[i*bg_w+j].x = x; bg[i*bg_w+j].y = y; bg[i*bg_w+j].z = z;
	}
	fclose(f);
}

int main(int argc, char *argv[])
{
	image_id = atoi(argv[1]);
	printf("%d\n",image_id);
	kFocusDist = 130.0 * image_id / 100.0;
	kFocusLength = 1.1 + (1.3 - 1.1) / 100.0 * image_id;
//	readbg("rainbow.ppm", bg1, bg1_w, bg1_h);
	readbg("colorful.ppm", bg2, bg2_w, bg2_h);
	
	for (int i=0; i<kImgH; i++)
	for (int j=0; j<kImgW; j++)
		calculateColor(kImgH-i, j);
	
	emitPhotons();
	return 0;
}
