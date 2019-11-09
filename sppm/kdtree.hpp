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
	int x,y;
	double current_tot_weight = 0;
	vec3d norm, color;
	Info(){}
	Info(const int &x_, const int &y_, const vec3d &norm_): x(x_), y(y_), norm(norm_)
	{
		color = vec3d();
	}
	void update(const vec3d &newcolor, vec3d *colormap)
	{
		color = color * current_tot_weight + newcolor;
		current_tot_weight += 1.0;
		color = color / current_tot_weight;
		colormap[int(x*kImgW+y)] = color;
	}
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
	
	void UpdatePointInSphere(const kdpoint3d &o,const double &r, const vec3d &norm, const vec3d &color, vec3d *colormap, int &modify)
	{
		double weight = std::max(0.0, std::abs(norm.dot(msg.norm)));
		double q[3];
		for(int i=0;i<3;i++)
			q[i]=std::max(pl.x[i],std::min(pr.x[i],o.x[i]));
		if(dist_kdpoint3d(kdpoint3d(q),o)>r)
			return;
		if(dist_kdpoint3d(p,o)<=r)
		{
			modify++;
			msg.update(color,colormap);
		}
		if(lc)
			lc->UpdatePointInSphere(o,r,norm,color,colormap,modify);
		if(rc)
			rc->UpdatePointInSphere(o,r,norm,color,colormap,modify);
	}
}S[kKDNodes];

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

void storePhoton(const int &x, const int &y, const vec3d &position, const vec3d &norm)
{
	tot_points++;
	S[tot_points].p=kdpoint3d(position);
	S[tot_points].msg=Info(x, y, norm);
	build(S+(tot_points&tot_points-1)+1,S+tot_points,0);
}

void resetPhotons()
{
	tot_points = 0;
}

int updatePhotonAround(const vec3d &position, const vec3d &norm, const vec3d &color, vec3d *colormap, double radius)
{
	kdpoint3d p = kdpoint3d(position);
	int modify = 0;
	for(int i=tot_points; i; i&=i-1)
		S[i].UpdatePointInSphere(p, radius, norm, color, colormap, modify);
	return modify;
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