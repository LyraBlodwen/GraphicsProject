#pragma once

#include "objects.h"
#include "vec.hpp"

const double kExposure = 1.1;
const double kRadius = 0.5;
const int kPhotons = 2147483647;
const int kStepPhotons = 4000000;
const int kKDNodes = 4100000;
const int kNumSphere = 16;
const int kBounces = 7;
const double kImgW = 2000;
const double kImgH = 2000;
const double kFocusDist = 100;
const double kFocusLength = 1.1;
const double kAparture = 3;
const double kRandShift = 1e-3;

Sphere spheres[] = {
	Sphere(vec3d( 1e5,40.8,130), 1e5, DIFF, vec3d(),vec3d(.725,.45,.45)),//Left
	Sphere(vec3d(-1e5+140,40.8,130), 1e5, DIFF, vec3d(),vec3d(.725,.45,.45)),//Right
	Sphere(vec3d(70,40.8, 1e5), 1e5, DIFF,     vec3d(),vec3d(.85,.85,.85)),//Back
	Sphere(vec3d(70,40.8,-1e5+170), 1e5, DIFF, vec3d(),vec3d()),//Front
	Sphere(vec3d(70, 1e5, 130), 1e5, DIFF,   vec3d(),vec3d(.85,.85,.85)),//Bottom
	Sphere(vec3d(70,-1e5+130,130), 1e5, DIFF, vec3d(),vec3d(.75,.75,.75)),//Top
	Sphere(vec3d(110, 60, 70), 10, DIFF, vec3d(), vec3d(.95,.95,.95)),//Light
	
	Sphere(vec3d(50,16,40), 16, DIFF,     vec3d(),vec3d(.8,.8,.8)),//Diff
	Sphere(vec3d(50,28,40), 12, DIFF,     vec3d(),vec3d(.75,.25,.25)),//Diff
	Sphere(vec3d(50,38,40), 13, DIFF,     vec3d(),vec3d(.75,.75,.75)),//Diff
	
//	Sphere(vec3d(65,60,20), 16, SPEC,     vec3d(),vec3d(1,1,1)*.999),//Mirr
//	Sphere(vec3d(68,20,75), 8, REFR,     vec3d(),vec3d(1,1,1)*.999),//Glas
//	Sphere(vec3d(25,40,70), 8, SPEC,     vec3d(),vec3d(1,1,1)*.999),//Mirr
//	Sphere(vec3d(35,80,30), 8, REFR,     vec3d(),vec3d(1,1,1)*.999),//Glas
	
	Sphere(vec3d(34,50,88), 5 , REFR,     vec3d(),vec3d(1,1,1)*.999),//Glas
//	Sphere(vec3d(83,23,38), 6, SPEC,     vec3d(),vec3d(1,1,1)*.999),//Mirr
//	Sphere(vec3d(55,80,43), 6, REFR,     vec3d(),vec3d(1,1,1)*.999),//Glas
	
	Sphere(vec3d(30,8,40), 8, REFR,     vec3d(),vec3d(1,1,1)*.999),//Glas
//	Sphere(vec3d(18,55,40), 8, REFR,     vec3d(),vec3d(1,1,1)*.999),//Glas
	Sphere(vec3d(25,34,70), 8, SPEC,     vec3d(),vec3d(1,1,1)*.999),//Mirr
	
	Sphere(vec3d(75,62,20), 20, SPEC,     vec3d(),vec3d(1,1,1)*.999),//Mirr
	Sphere(vec3d(65,20,75), 6, REFR,     vec3d(),vec3d(1,1,1)*.999),//Glas
	Sphere(vec3d(78,30,20), 8, SPEC,     vec3d(),vec3d(1,1,1)*.999),//Mirr
};

//int bez_n = 3;
//double bx[]={0,10,10,0};
//double by[]={20,10,-10,-20};
//bezier beziercurve(bez_n,bx,by);
//BezierRotate BezObj(beziercurve, vec3d(80, 40, 70), SPEC);

int bez_n = 5;
double bx[]={0, 5.96, 9.20, 11.81, 15.8, 0};
double by[]={35, 29.83, 24.08, 13.73, 10, 10};

//double bx[]={0, 5.96, 9.20, 11.81, 15.8, 0};
//double by[]={40, 33.79, 26.89, 14.47, 10, 10};

bezier beziercurve(bez_n,bx,by);
BezierRotate BezObj(beziercurve, vec3d(51, 35, 40), acos(-1)/180.0 * -15, SPEC);

vec3d Light(70, 81.5, 70);