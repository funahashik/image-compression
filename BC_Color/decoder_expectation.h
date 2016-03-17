int Fukugou_Chi_Point(int point,int k);
int **Fukugou_Chi_Image(int **input_image,int width,int height,int k);
int Fukugou_Kitaichi_Point_After(int point,int k);
int **Fukugou_Kitaichi_Image_After(int **input_image,int width,int height,int k);
int ***Fukugou_Kitaichi_RGB_Image_After(int ***input_image,int width,int height,int k);
int Fukugou_Kitaichi_Point_Before(int point,int k);
int **Fukugou_Kitaichi_Image_Before(int **input_image,int width,int height,int k);

int Fukugou_Chi_Point(int point,int k)
{
  int f;

  f=1<<(k+1);
  f-=1;

  f=point | f;
  //printf("%d",f);
  return f;
}

int **Fukugou_Chi_Image(int **input_image,int width,int height,int k)
{//Fugou_Kitaichi

  int i,j;
  //char name[30];

  int **KthBitmap;
  KthBitmap=GetImgMemory(width,height);

  for(i=0;i<height;++i)
    {
      for(j=0;j<width;++j)
	{
	  KthBitmap[i][j]=1<<(k+1);
	  KthBitmap[i][j]-=1;
	  KthBitmap[i][j]=input_image[i][j] | KthBitmap[i][j];
	}
    }

  return KthBitmap;
}

//Mitaato
int Fukugou_Kitaichi_Point_After(int point,int k)
{
  int f;
  if(k==0)
    {
      return point;
    }
  f=point>>k;
  f=f<<1;
  f+=1;
  f=f<<(k-1);
  f-=1;

  return f;
}

//Mitaato
int **Fukugou_Kitaichi_Image_After(int **input_image,int width,int height,int k)
{//Fugou_Kitaichi

  int i,j;
  //char name[30];

  int **KthBitmap;
  if(k==0)
    {
      return input_image;
    }
  KthBitmap=GetImgMemory(width,height);



  for(i=0;i<height;++i)
    {
      for(j=0;j<width;++j)
	{

	  KthBitmap[i][j]=input_image[i][j]>>k;
	  KthBitmap[i][j]=KthBitmap[i][j]<<1;
	  KthBitmap[i][j]+=1;
	  KthBitmap[i][j]=KthBitmap[i][j]<<(k-1);
	  KthBitmap[i][j]-=1;

	}
    }

  return KthBitmap;
}

int ***Fukugou_Kitaichi_RGB_Image_After(int ***input_image,int width,int height,int k)
{//Fugou_Kitaichi

  int i,j,l;
  //char name[30];

  int ***KthBitmap;
  if(k==0)
    {
      return input_image;
    }
  KthBitmap=GetRGBImgMemory(width,height);


  for(l=0;l<3;++l)
  {
    for(i=0;i<height;++i)
    {
      for(j=0;j<width;++j)
	  {
	    KthBitmap[l][i][j]=input_image[l][i][j]>>k;
	    KthBitmap[l][i][j]=KthBitmap[l][i][j]<<1;
	    KthBitmap[l][i][j]+=1;
	    KthBitmap[l][i][j]=KthBitmap[l][i][j]<<(k-1);
	    KthBitmap[l][i][j]-=1;
	  }
	}
  }
  return KthBitmap;
}
//Mirumae
int Fukugou_Kitaichi_Point_Before(int point,int k)
{
  int f;

  f=point>>(k+1);//Kesu
  f=f<<1;//??1
  f+=1;
  f=f<<k;//??1000...00
  f-=1;//??0111...11

  return f;
}

//Mirumae
int **Fukugou_Kitaichi_Image_Before(int **input_image,int width,int height,int k)
{//Fugou_Kitaichi

  int i,j;
  //char name[30];

  int **KthBitmap;
  KthBitmap=GetImgMemory(width,height);

  for(i=0;i<height;++i)
    {
      for(j=0;j<width;++j)
	{

	  KthBitmap[i][j]=input_image[i][j]>>(k+1);
	  KthBitmap[i][j]=KthBitmap[i][j]<<1;
	  KthBitmap[i][j]+=1;
	  KthBitmap[i][j]=KthBitmap[i][j]<<k;
	  KthBitmap[i][j]-=1;

	}
    }

  return KthBitmap;
}
