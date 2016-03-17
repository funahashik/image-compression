void Forward_EMN(int ***before,int ***after,int width,int height);
void Inverse_EMN(int ***before,int ***after,int width,int height);
int ***ABS_Image(int ***Image,int width,int height)
{
	int ***RGB;
	int i,j,k;

	RGB=GetRGBImgMemory(width,height);

	for(i=0;i<width;++i)
	{
		for(j=0;j<height;++j)
		{
			for(k=0;k<3;++k)
			{
				RGB[k][j][i]=abs(Image[k][j][i]);
			}
		}
	}
	return RGB;
}
void ABS_Image2(int ***Image2,int ***Image,int width,int height)
{
	int i,j,k;

	for(i=0;i<width;++i)
	{
		for(j=0;j<height;++j)
		{
			for(k=0;k<3;++k)
			{
				Image2[k][j][i]=abs(Image[k][j][i]);
			}
		}
	}
	//return ;
}

void Forward_Lif_EMN(int ***after,int width,int height)
{
	int i,j;

	for(i=0;i<width;++i)
	{
		for(j=0;j<height;++j)
		{

			//R-=G
			after[0][j][i]-=after[1][j][i];
			//B-=G
			after[2][j][i]-=after[1][j][i];
			//G+=(R+B)/3
			after[1][j][i]+=(after[0][j][i]+after[2][j][i])/3;

		}
	}
	return;
}

void Inverse_Lif_EMN(int ***after,int width,int height)
{
	int i,j;

	for(i=0;i<width;++i)
	{
		for(j=0;j<height;++j)
		{

			//G-=(R+B)/3
			after[1][j][i]-=(after[0][j][i]+after[2][j][i])/3;
			//B+=G
			after[2][j][i]+=after[1][j][i];
			//R+=G
			after[0][j][i]+=after[1][j][i];

		}
	}
	return;
}

void Forward_EMN(int ***before,int ***after,int width,int height)
{
	int i,j;

	for(i=0;i<width;++i)
	{
		for(j=0;j<height;++j)
		{

			//E=(R+G+B)/3
			after[0][j][i]=(before[0][j][i]+before[1][j][i]+before[2][j][i])/3;
			//M=R-G
			after[1][j][i]=before[0][j][i]-before[1][j][i];
			//N=B-G
			after[2][j][i]=before[2][j][i]-before[0][j][i];


		}
	}
	return;
}

void Inverse_EMN(int ***before,int ***after,int width,int height)
{
	int i,j;

	for(i=0;i<width;++i)
	{
		for(j=0;j<height;++j)
		{

			//R=E-M
			after[0][j][i]=before[0][j][i]+(before[1][j][i]-before[2][j][i])/3;
			//G=-E/2+3N/2
			after[1][j][i]=before[0][j][i]+(-2*before[1][j][i]-before[2][j][i])/3;
			//B=-E/2+M+3N/2
			after[2][j][i]=before[0][j][i]+(before[1][j][i]+2*before[2][j][i])/3;

		}
	}
	return;
}