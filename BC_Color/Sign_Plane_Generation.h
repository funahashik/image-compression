int ***Sign_Bit_Plane_PPM(int ***input_image,int width,int height);

int ***Sign_Bit_Plane_PPM(int ***input_image,int width,int height)
{
  int i,j,l; 
  int ***Bitmap;

  //char name[30];
  Bitmap=GetRGBImgMemory(width,height);

  for(l=0;l<3;++l)
    {
      for(i=0;i<height;++i)
	  {
	     for(j=0;j<width;++j)
	     {

			 Bitmap[l][i][j]=(input_image[l][i][j]<0 ? 0:1);

	     }
	  }
    }
  return Bitmap;
}