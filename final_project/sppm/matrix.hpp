#pragma once

const int MAT_SIZE = 10;

struct matrix
{
	int r,c;
	double w[MAT_SIZE][MAT_SIZE];
	
	matrix operator-(const matrix &f)const		//matrix-matrix
	{
		matrix g;
		g.r=r;g.c=c;
		for(int i=1;i<=r;i++)
			for(int j=1;j<=c;j++)
				g.w[i][j]=w[i][j]-f.w[i][j];
		return g;
	}
	
	matrix operator*(const matrix &f)const	//matrix*matrix
	{
		matrix g;
		memset(g.w,0,sizeof(w));
		g.r=r;g.c=f.c;
		int i,j,k;
		for(i=1;i<=r;i++)
			for(j=1;j<=f.c;j++)
				for(k=1;k<=c;k++)
					g.w[i][j]+=w[i][k]*f.w[k][j];
		return g;
	}
	
	void inverse()	//get A's inverse using Guass elimination
	{
		int i,j,k,n=r;
		for(i=1;i<=n;i++)
		w[i][i+n]=1;
		for(i=1;i<=n;i++)
		{
			double t=1.0/w[i][i];
			for(j=i;j<=2*n;j++)
				w[i][j]*=t;
			for(j=i+1;j<=n;j++)
			{
				double t=w[j][i]/w[i][i];
				for(k=i;k<=2*n;k++)
				{
					w[j][k]-=t*w[i][k];
				}
			}
		}
		for(i=n;i>=1;i--)
		{
			for(j=i-1;j>=1;j--)
			{
				double t=w[j][i]/w[i][i];
				for(k=i;k<=2*n;k++)
				{
					w[j][k]-=t*w[i][k];
				}
			}
		}
		for(i=1;i<=n;i++)
			for(j=1;j<=n;j++)
			{
				w[i][j]=w[i][j+n];
				w[i][j+n]=0;
			}
	}
	
	void pr(std::string s="")	//print matrix
	{
		std::cout<<s<<':'<<std::endl;
		puts("-------------------");
		for(int i=1;i<=r;i++)
		{
			for(int j=1;j<=c;j++)
			printf("%10.5lf",w[i][j]);
			puts("");
		}
		puts("-------------------");
	}
};
