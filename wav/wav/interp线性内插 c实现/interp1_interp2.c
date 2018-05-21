/*写此文件的目的是为了和网友分享MATLAB中interp1与interp2转为c的功能。
注：在网上找了很久没找到，最后下定决心自己写。*/
/**************线性插值***********
(x0,y0)->(x1,y1)
*********************************/
double inter_linear(double x0,double x1,double y0,double y1,double x) 
{ 
	double a0,a1,y; 
	a0=(x-x1)/(x0-x1); 
	a1=(x-x0)/(x1-x0); 
	y=a0*y0+a1*y1; 

	return (y); 
} 
/**************一维线性插值*************
x[],y[]为长度为n的向量（一维数组）
***************************************/
double interp1(double x[],double y[],int n,double pp)
{
	double logx[18],logp,z;
	int i,tmp;
	for(i = 0;i<n;i++)
	{
		logx[i] = log(x[i]);
	}
	logp = log(pp);
	for(i = 0;i<n;i++)
	{
		if((logp>=logx[i])&&(logp<logx[i+1]))
		{
			tmp = i;
			break;
		}
	}
	//printf("i = %d\n",i);
	//if(i == 0)
	//{
	//	z = y[0];
	//	return z;
	//}
	if(i == 18)
	{
		z = y[i-1];
		return z;
	}
	z = inter_linear(logx[i],logx[i+1],y[i],y[i+1],logp);

	return z;
}
/**************双线性插值************
x[],y[]为长度为n的向量（一维数组），z为矩阵（对应的二维数组，此处为数组指针）
************************************/
double interp2(double x[],double y[],double **z,int m,int n,
				double a,double b)
{
	int i,j,tmpi,tmpj;
	double w,w1,w2;
	tmpi = 0;
	tmpj = 0;
	w = 0.0;
	/***********确定(a,b)所在位置***********/
	for(i=0; i<m; i++)                    //确定a在x轴的位置
	{
		if( (a<=x[i])&&(a>x[i+1]) )     //x为降序  
		{
			tmpi = i;
			break;
		}
		//if( (a>=x[i])&&(a<x[i+1]) )       //x为升序
		//{
		//	tmpi = i;
		//	break;
		//}
	}
	for(j=0; j<n; j++)                    //确定b在y轴的位置
	{
		if( (b>=y[j])&&(b<y[j+1]) )       
		{
			tmpj = j;
			break;
		}
	}
	/**************x方向进行插值*************/
	if(x[tmpi] == a)
	{
		//取网格节点值
		w1 = z[tmpi][tmpj];
		w2 = z[tmpi][tmpj+1];

		/*******y方向进行插值********/
		if(y[tmpj] == b)
		{
			//取网格节点值
			w = w1;
		}
		else
		{
			//进行插值(y)
			w = inter_linear(y[tmpj],y[tmpj+1],w1,w2,b);
		}
		/***************************/
	}
	else
	{
		//进行插值(x)
		w1 = inter_linear(x[tmpi],x[tmpi+1],z[tmpi][tmpj],z[tmpi+1][tmpj],a);
		w2 = inter_linear(x[tmpi],x[tmpi+1],z[tmpi][tmpj+1],z[tmpi+1][tmpj+1],a);

		/*******y方向进行插值********/
		if(y[tmpj] == b)
		{
			//取网格节点值
			w = w1;
		}
		else
		{
			//进行插值(y)
			w = inter_linear(y[tmpj],y[tmpj+1],w1,w2,b);
		}
		/****************************/
	}

	return w;
}
//C方法读取txt文件数据
int read_data(const char * path, double a[])
{
	/***定义变量***/
	double f_temp;
	int i;
	//int j;
	FILE *fp;

	/*****打开文件，读取数据*****/
	fp = fopen(path, "rb");
	if(fp == NULL)						//如果失败了
	{
		printf("打开文件失败!");
		return -1;
	}
	i = 0;
	while(fscanf(fp, "%lf\n", &f_temp) == 1)
	{
		a[i] = f_temp;
		i++;
	}
	//printf("i = %d\n",i);
	///*******打印数组内容*******/
	//for( j = 0; j<i; j++)
	//{
	//	printf("a[%d", j);
	//	printf("] = ");
	//	printf("%14.14f\n", a[j]);
	//}

	return 0;
}

//C方法读取txt文件数据 2
int read_data2(const char * path, double *a)
{
	/***定义变量***/
	double f_temp;
	int i;
	//int j;
	FILE *fp;

	/*****打开文件，读取数据*****/
	fp = fopen(path, "rb");
	if(fp == NULL)						//如果失败了
	{
		printf("打开文件失败!");
		return -1;
	}
	i = 0;
	while(fscanf(fp, "%lf\n", &f_temp) == 1)
	{
		*a = f_temp;
		a = a+1;
	}
	//printf("i = %d\n",i);
	///*******打印数组内容*******/
	//for( j = 0; j<i; j++)
	//{
	//	printf("a[%d", j);
	//	printf("] = ");
	//	printf("%14.14f\n", a[j]);
	//}

	return 0;
}

