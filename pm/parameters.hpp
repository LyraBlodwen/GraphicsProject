#pragma once

#include "objects.h"
#include "vec.hpp"

const double kExposure = 3200;//3200;
const double kRadius = 20;//20;
const int kPhotons = 40000;//40000;
const int kNumSphere = 9;
const int kBounces = 7;
const double kImgW = 640;
const double kImgH = 640;

Sphere spheres[] = {
	Sphere(vec3d( 1e5+1,40.8,81.6), 1e5, DIFF, vec3d(),vec3d(.45,.25,.25)),//Left
	Sphere(vec3d(-1e5+99,40.8,81.6), 1e5, DIFF, vec3d(),vec3d(.25,.35,.5)),//Rght
	Sphere(vec3d(50,40.8, 1e5), 1e5, DIFF,     vec3d(),vec3d(.75,.75,.55)),//Back
	Sphere(vec3d(50,40.8,-1e5+170), 1e5, DIFF, vec3d(),vec3d()),//Frnt
	Sphere(vec3d(50, 1e5, 81.6), 1e5, DIFF,   vec3d(),vec3d(.75,.75,.75)),//Botm
	Sphere(vec3d(50,-1e5+81.6,81.6), 1e5, DIFF, vec3d(),vec3d(.2,.2,.2)),//Top
	Sphere(vec3d(73,28,78), 14, REFR,     vec3d(),vec3d(1,1,1)*.999),//Glas
	Sphere(vec3d(27,16,20), 16, SPEC,     vec3d(),vec3d(.8,.8,.8)),//Mirr
	Sphere(vec3d(27,37,20), 13, SPEC,     vec3d(),vec3d(.75,.75,.75)),//Diff
//	Sphere(vec3d(23,40,29), 3, SPEC,     vec3d(),vec3d()),//Mirr
//	Sphere(vec3d(31,40,29), 3, SPEC,     vec3d(),vec3d()),//Mirr
//	Sphere(vec3d(25,35,28.6), 4, DIFF,     vec3d(),vec3d()),//Diff
//	Sphere(vec3d(26,34.85,28.73), 4, DIFF,     vec3d(),vec3d()),//Diff
//	Sphere(vec3d(27,34.7,28.75), 4, DIFF,     vec3d(),vec3d()),//Diff
//	Sphere(vec3d(28,34.85,28.73), 4, DIFF,     vec3d(),vec3d()),//Diff
//	Sphere(vec3d(29,35,28.6), 4, DIFF,     vec3d(),vec3d()),//Diff
};

vec3d Light(50, 81.6, 70);