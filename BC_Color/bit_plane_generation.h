int Nth_Bit_Point_PPM(int point,int k);
void Nth_Bit_Plane_PPM2(int ***KthBitmap,int ***input_image,int width,int height,int k);
int ***Nth_Bit_Plane_PPM(int ***input_image,int width,int height,int k);

int Nth_Bit_Point_PGM_ver2(int point,int k);
int **Nth_Bit_Plane_PGM_ver2(int **input_image,int width,int height,int k);

int **Nth_Bit_Plane_PGM(int **input_image,int width,int height,int k);
//int ***Nth_BitPlane_PPM(int ***input_image,int width,int height,int k);

int **NthBitMapGeneration_PGM(char *filename,int **input_image,int width,int height,int k,int write_question);
int ***NthBitMapGeneration_PPM(char *filename,int ***input_image,int width,int height,int k,int write_question);

int Nth_Bit_Point_PPM(int point,int k)//Tsukawanakuteyoi
{
  int a,saguri=1;
  //int f;
  saguri=saguri << k;

  //char name[30];
  a=point & saguri;

  a=a>>k;

  return a;
}
//0-7th_Bit_Plane_Generationable
//7:upper_ground,,0:lower_ground
void Nth_Bit_Plane_PPM2(int ***KthBitmap,int ***input_image,int width,int height,int k)
{

  int i,j,l;
  int saguri=1;
  //int ***KthBitmap;

  //char name[30];
  //KthBitmap=GetRGBImgMemory(width,height);
  //int f;
  saguri=saguri << k;

  for(l=0;l<3;++l)
    {
      for(i=0;i<height;++i)
	  {
	     for(j=0;j<width;++j)
	     {
	      //f=input_image[i][j];

	      KthBitmap[l][i][j]=input_image[l][i][j] & saguri;

	      KthBitmap[l][i][j]=KthBitmap[l][i][j]>>k;

	     }
	  }
    }
  /*
  if(write_question==1)
    {
      sprintf(name,"%s_%d_bitmap.pgm",filename,k);

      PGMWrite(name,KthBitmap, width,height);
    }
  */
  return;
  //return KthBitmap;
}

int ***Nth_Bit_Plane_PPM(int ***input_image,int width,int height,int k)
{

  int i,j,l;
  int saguri=1;
  int ***KthBitmap;

  //char name[30];
  KthBitmap=GetRGBImgMemory(width,height);
  //int f;
  saguri=saguri << k;

  for(l=0;l<3;++l)
    {
      for(i=0;i<height;++i)
	  {
	     for(j=0;j<width;++j)
	     {
	      //f=input_image[i][j];

	      KthBitmap[l][i][j]=input_image[l][i][j] & saguri;

	      KthBitmap[l][i][j]=KthBitmap[l][i][j]>>k;

	     }
	  }
    }
  /*
  if(write_question==1)
    {
      sprintf(name,"%s_%d_bitmap.pgm",filename,k);

      PGMWrite(name,KthBitmap, width,height);
    }
  */
  return KthBitmap;
}


int Nth_Bit_Point_PGM_ver2(int point,int k)
{
  int a,saguri=1;
  //int f;
  saguri=saguri << k;

  //char name[30];
  a=point & saguri;

  a=a>>k;

  return a;
}
//0-7th_Bit_Plane_Generationable
//7:upper_ground,,0:lower_ground


int **Nth_Bit_Plane_PGM_ver2(int **input_image,int width,int height,int k)
{

  int i,j;
  int saguri=1;


  int **KthBitmap;

  //int f;
  saguri=saguri << k;

  //char name[30];
  KthBitmap=GetImgMemory(width,height);

  for(i=0;i<height;++i)
    {
      for(j=0;j<width;++j)
	{
	  //f=input_image[i][j];

	  KthBitmap[i][j]=input_image[i][j] & saguri;

	  KthBitmap[i][j]=KthBitmap[i][j]>>k;

	}
    }
  /*
  if(write_question==1)
    {
      sprintf(name,"%s_%d_bitmap.pgm",filename,k);

      PGMWrite(name,KthBitmap, width,height);
    }
  */
  return KthBitmap;
}

int **Nth_Bit_Plane_PGM(int **input_image,int width,int height,int k)
{

  int i,j;
  int saguri=1;


  int **KthBitmap;
  //int f;
  saguri=saguri << k;

  //char name[30];
  KthBitmap=GetImgMemory(width,height);

  for(i=0;i<height;++i)
    {
      for(j=0;j<width;++j)
	{
	  //f=input_image[i][j];

	  KthBitmap[i][j]=input_image[i][j] & saguri;

	  KthBitmap[i][j]=KthBitmap[i][j]<<(7-k);

	}
    }
  /*
  if(write_question==1)
    {
      sprintf(name,"%s_%d_bitmap.pgm",filename,k);

      PGMWrite(name,KthBitmap, width,height);
    }
  */
  return KthBitmap;
}




int **NthBitMapGeneration_PGM(char *filename,int **input_image,int width,int height,int k,int write_question)
{

  int i,j;
  int saguri=1;


  char name[30];

  int **KthBitmap;

  //int f;
  saguri=saguri << k;
  KthBitmap=GetImgMemory(width,height);

  for(i=0;i<height;++i)
    {
      for(j=0;j<width;++j)
	{
	  //f=input_image[i][j];

	  KthBitmap[i][j]=input_image[i][j] & saguri;

	  KthBitmap[i][j]=KthBitmap[i][j]<<(7-k);

	}
    }

  if(write_question==1)
    {
      sprintf(name,"%s_%d_bitmap.pgm",filename,k);

      PGMWrite(name,KthBitmap, width,height);
    }

  return KthBitmap;
}

int ***NthBitMapGeneration_PPM(char *filename,int ***input_image,int width,int height,int k,int write_question)
{

  int i,j,l;
  int saguri=1;
  char name[30];
  int ***KthBitmap;

  //int f;
  saguri=saguri << k;

  KthBitmap=GetRGBImgMemory(width,height);

  for(l=0;l<3;l++)
    {
      for(i=0;i<height;++i)
	{
	  for(j=0;j<width;++j)
	    {
	      //f=input_image[i][j];

	      KthBitmap[l][i][j]=input_image[l][i][j] & saguri;

	      KthBitmap[l][i][j]=KthBitmap[l][i][j]<<(7-k);

	    }
	}
    }

  if(write_question==1)
    {
      sprintf(name,"%s_%d_bitmap.ppm",filename,k);

      PPMWrite(name,KthBitmap, width,height);
    }

  return KthBitmap;
}
