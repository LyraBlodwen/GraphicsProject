#pragma once

#include <iostream>
#include <cstring>
#include <algorithm>
#include <vector>
#include <cmath>
#include "parameters.hpp"

using std::vector;
using std::pair;
using std::make_pair;

template<class T> inline bool ten(T &x,const T &y){return y<x?x=y,1:0;}
template<class T> inline bool rel(T &x,const T &y){return x<y?x=y,1:0;}

const int KD_DIM=3;

double sqr(const double &xx){return xx*xx;}

struct kdpoint3d
{
	double x[KD_DIM];
	kdpoint3d(){}
	kdpoint3d(const vec3d &pos)
	{
		x[0] = pos.x; x[1] = pos.y; x[2] = pos.z;
	}
	
	kdpoint3d(const double* pos)
	{
		memcpy(x,pos,sizeof(x));
	}
	
	kdpoint3d(const double &a,const double &b,const double &c)
	{
		x[0]=a,x[1]=b,x[2]=c;
	}
	
	kdpoint3d operator+(const kdpoint3d &o)const
	{
		return kdpoint3d(x[0]+o.x[0],x[1]+o.x[1],x[2]+o.x[2]);
	}
	kdpoint3d operator-(const kdpoint3d &o)const
	{
		return kdpoint3d(x[0]-o.x[0],x[1]-o.x[1],x[2]-o.x[2]);
	}
};

struct Info
{
	vec3d direction, color;
	Info(){}
	Info(vec3d direction_, vec3d color_): direction(direction_), color(color_){}
};

double dist_kdpoint3d(const kdpoint3d &a,const kdpoint3d &b)
{
	double s=0;
	for(int i=0;i<3;i++)
		s+=sqr(a.x[i]-b.x[i]);
	return sqrt(s);
}

struct node
{
	kdpoint3d p;
	Info msg;
	kdpoint3d pl,pr;
	node *lc,*rc;
	
	void GetPointInSphere(const kdpoint3d &o,const double &r,vector<pair<kdpoint3d,Info> > &res)
	{
		double q[3];
		for(int i=0;i<3;i++)
			q[i]=std::max(pl.x[i],std::min(pr.x[i],o.x[i]));
		if(dist_kdpoint3d(kdpoint3d(q),o)>r)
			return;
		if(dist_kdpoint3d(p,o)<=r)
			res.push_back(make_pair(p,msg));
		if(lc)
			lc->GetPointInSphere(o,r,res);
		if(rc)
			rc->GetPointInSphere(o,r,res);
	}
	
}S[400005];

int cmp_d;

bool cmp_dim(const node &a,const node &b)
{return a.p.x[cmp_d]<b.p.x[cmp_d];}

node* build(node *lp,node *rp,int d)
{
	if(lp>rp)return NULL;
	node *mp=lp+(rp-lp)/2;
	cmp_d=d;
	std::nth_element(lp,mp,rp+1,cmp_dim);
	std::swap(*mp,*rp);
	rp->lc=build(lp,mp-1,(d+1)%3);
	rp->rc=build(mp,rp-1,(d+1)%3);
	rp->pl=rp->pr=rp->p;
	for(int i=0;i<3;i++)
	{
		if(rp->lc)
		{
			ten(rp->pl.x[i],rp->lc->pl.x[i]);
			rel(rp->pr.x[i],rp->lc->pr.x[i]);
		}
		if(rp->rc)
		{
			ten(rp->pl.x[i],rp->rc->pl.x[i]);
			rel(rp->pr.x[i],rp->rc->pr.x[i]);
		}
	}
	return rp;
}

int tot_points = 0;

void storePhoton(const vec3d &position, const vec3d &direction, const vec3d &color)
{
//	printf("%.3lf %.3lf %.3lf, %.3lf %.3lf %.3lf, %.3lf %.3lf %.3lf\n", position.x, position.y, position.z,direction.x,direction.y,direction.z,color.x, color.y, color.z);
	tot_points++;
	S[tot_points].p=kdpoint3d(position);
	S[tot_points].msg=Info(direction, color);
	build(S+(tot_points&tot_points-1)+1,S+tot_points,0);
}

//vec3d cmp_position;
//
//bool cmp_dis(const pair<kdpoint3d, Info> &p, const pair<kdpoint3d, Info> &q)
//{
//	return dist_kdpoint3d(cmp_position, p.first) < dist_kdpoint3d(cmp_position, q.first);
//}

vec3d getPhotonAround(const vec3d &position, const vec3d &norm)
{
	kdpoint3d p = kdpoint3d(position);
//	cmp_position = position;
	vector<pair<kdpoint3d, Info> > res;
	for(int i=tot_points; i; i&=i-1)
		S[i].GetPointInSphere(p, kRadius, res);
	vec3d color;
//	printf("#%d\n",res.size());
//	norm.pr();
//	sort(res.begin(), res.end(), cmp_dis);
	int len = res.size();
//	if (len>2000) len = 2000;
	vec3d avg = vec3d();
	for(int i=0; i<len; i++)
	{
//		if (i>10 && (avg * (1.0 / i)).dist(res[i].second.color)>0.5) continue;
		vec3d currentPos(res[i].first.x[0], res[i].first.x[1], res[i].first.x[2]);
		double weight = std::max(0.0, std::abs(norm.dot(res[i].second.direction)));
		weight *= (kRadius - position.dist(currentPos)) / kExposure;
//		if (norm.y > 0.99) printf("%.3lf\n",-norm.dot(res[i].second.direction));
//		avg = avg + res[i].second.color;
		color = color + res[i].second.color * weight;
	}
	color = (color * 255.0).min(vec3d(255,255,255));
	if (position.y>81.5 && position.x>=40 && position.x<=60 && position.z>=60 && position.z<=80) color = vec3d(1,1,1) * 255;
//	printf("%d %d %d\n",int(color.x), int(color.y),int(color.z));
	return color;
}

/*
void Ins(const kdpoint3d &p,const Info &msg)
{
	tot_points++;
	S[tot_points].p=p;
	S[tot_points].msg=msg;
	build(S+(tot_points&tot_points-1)+1,S+tot_points,0);
}

void Ask(const kdpoint3d &p,const double &r)
{
	vector<pair<kdpoint3d, Info> > res;
	for(int i=tot_points;i;i&=i-1)
		S[i].GetPointInSphere(p,r,res);
	for(int i=0;i<res.size();i++)
		cout<<res[i].second.id<<' ';
	cout<<endl;
}

int main(int argc, char *argv[])
{
	int Q,idx=0;
	cin>>Q;
	while(Q--)
	{
		int tp;
		double x[3],d;
		scanf("%d",&tp);
		if(tp==1)
		{
			scanf("%lf%lf%lf",x,x+1,x+2);
			Ins(kdpoint3d(x),Info(++idx));
		}
		else
		{
			scanf("%lf%lf%lf%lf",x,x+1,x+2,&d);
			Ask(kdpoint3d(x),d);
		}
	}
}
*/