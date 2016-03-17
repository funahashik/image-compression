//
//This Source Code is reported follwing conferece and paper(Color Image Version)
//K.Kikuchi, K. Funahashi, and S.Muramatsu, Simple Bit-plane Coding for Lossless Image Compression and Extended FUnctionalities, 27th PCS(PCS 2009)
//K.Funahashi, K.Kikuchi, and S.Muramatsu, Progressive Bitplane Coding for Lossless Image Compression  [in Japanese], Technical report of IEICE. VLD 108(106), 23-28, 2008-06-19
//K.Kikuchi, R.Abe, and S.Muramatsu, Simple Bitplane Coding and Its Application to Multi-Functional Image Compression, EICE Transactions on Fundamentals of Electronics, Communications and Computer Sciences
//
/*Context Model Only Most Upper Plane*/
/*Not Use Filterling Method for decision context model*/
//Context-Changing
//For Color
/*

7-6
| | |  | |
| |#|# |#|
| |#|X+|#|
| |#|# |#|

3-1
| | |  | |
| |#|# |#|
| |#|X |#|
| |#|# |#|
lena


*/
#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>

#include<math.h>
#include<string.h>
#include<time.h>

#include"range.h"
#include"Active_Memory_Range_Coder.h"

#include"ppm_pgm.h"
#include"EMN_Color_Trans.h"

#include"Sign_Plane_Generation.h"

#include"bit_plane_generation.h"
#include"decoder_expectation.h"

#include"2DExpansion.h"

#include"Context_Generator_bilevel.h"

//From rcenc0.c
#define SYMS 2

int Ex_Sy2(int size,int num)
{
  // For WS,WA
  // num :refer_point
  // size:(size = width or height)

  //dcba|abcd|dcba

  if(num >= size)
  {
	  //x-2(x-(size-1))=2(w-1)-x
    return (size-1)*2 - num+1;
  }
  if(num < 0)
  {
    return abs(num)-1;
  }
  return num;
}

void coding_ppm(char *filename, char *string);

void coding_ppm(char *filename, char *string)
{
  int i,j,k,c;
  int width,height;
  int ***ppm,***image1;
  int point;
  float evaluation_rate;

  int model_num,context_size;

  Uint buf=0;

  Uint **ftbl;
  rc_model *rcm;

  //From rcenc0.c
  rc_encoder rc;
  FILE *fp;//For_Writing
  Uint fsize, l;

 //Encoded Image Decision
  image1=PPMRead(filename, &width, &height);
  ppm=GetRGBImgMemory(width,height);

  if((fp=fopen(string,"wb"))==NULL)
    {
      printf("Can't Open\n");
      exit(1);
    }

  //Initialization Writing

  fsize=(Uint)(width*height*8+1);

  putc(width & 0xff, fp);  //Save width
  putc((width >> 8) & 0xff, fp);
  putc((width >> 16) & 0xff, fp);
  putc((width >> 24) & 0xff, fp);
  
  putc(height & 0xff, fp);  //Save height 
  putc((height >> 8) & 0xff, fp);
  putc((height >> 16) & 0xff, fp);
  putc((height >> 24) & 0xff, fp);


  StartEncode (&rc, fp, NULL, 0);
  printf("Encoding...\n");
  fflush(stdout);

  /*** Highest Bit Plane Processing 7  ***/



#define MODEL_NUM1 2048

  //RANGE_MODEL Active preparation
  
  //Color Transform : GMN
  //image1=LMN_ForwardColorTrans_Kai2(ppm,width,height);
  Forward_Lif_EMN(image1, width,height);

  ppm=Sign_Bit_Plane_PPM(image1,width,height);

  /******** Sign Coding********/
  ftbl=GetFreqTable(SYMS,MODEL_NUM1);
  rcm=Get_RC_Model(MODEL_NUM1);
 
  model_num=128;
  context_size=7;

  printf("Sign: \n");
  for(c=0;c<3;c+=2)
  {

      //Probability Model Initialization_1
      for(i=0;i<model_num;++i)
	  {
	    l = ModelInit (&rcm[i], SYMS, ftbl[i], NULL, 2, (Uint)1<<14, ADAPT);
	    if (l != RC_OK) 
	    {
	      fprintf(stderr, "Error: maxFreq or totFreq > 65536\n");
	      exit(1);
	    }
	  }
      //Coding
      for(i=0;i<height;++i)
	  {
	    for(j=0;j<width;++j)
	    {
			//Context_Model_Decision

	        l=context_generator_bit(context_size,
				      Zero_Ex_Operator(i-1,height)*ppm[c][Ex_Sy(height,i-1)][j],

				      Zero_Ex_Operator(j-1,width)*ppm[c][i][Ex_Sy(width,j-1)],

				      Zero_Ex_Operator(i-1,height)*Zero_Ex_Operator(j-1,width)
				      *ppm[c][Ex_Sy(height,i-1)][Ex_Sy(width,j-1)],

				      Zero_Ex_Operator(i-1,height)*Zero_Ex_Operator(j+1,width)
				      *ppm[c][Ex_Sy(height,i-1)][Ex_Sy(width,j+1)],		
		  
				      Zero_Ex_Operator(i-2,height)*ppm[c][Ex_Sy(height,i-2)][j],

				      Zero_Ex_Operator(j-2,width)*ppm[c][i][Ex_Sy(width,j-2)],

				      Zero_Ex_Operator(i-1,height)*Zero_Ex_Operator(j+2,width)
					  *ppm[c][Ex_Sy(height,i-1)][Ex_Sy(width,j+2)]
				      );

	        EncodeSymbol (&rc, &rcm[l], ppm[c][i][j]);
	        if (rc.error != RC_OK) break;
	    }
	  }
      //Rate Calculation
      i=8*(int)(rc.passed-buf);
      evaluation_rate=(float)(i)/(float)(width*height);
      printf("componet: %d, cumurative =%6lu, current=%6lu, ( bytes ), %f(bpp)\n",c,rc.passed,rc.passed-buf,evaluation_rate);

      buf=rc.passed;  
  }

  /************Absolute Coding*************/
  printf("Absolute:\n");
  ppm=ABS_Image(image1,width,height);

  //Processing Image Decision
  image1=Nth_Bit_Plane_PPM(ppm,width,height,7);

  for(c=0;c<3;++c)
  {
      //Probability Model Initialization_1
      for(i=0;i<model_num;++i)
	  {
	    l = ModelInit (&rcm[i], SYMS, ftbl[i], NULL, 2, (Uint)1<<14, ADAPT);
	    if (l != RC_OK) 
	    {
	      fprintf(stderr, "Error: maxFreq or totFreq > 65536\n");
	      exit(1);
	    }
	  }
  
    //Coding
    for(i=0;i<height;++i)
	{
	  for(j=0;j<width;++j)
	  {
	      //Context_Model_Decision

	      l=context_generator_bit(context_size,
				      Zero_Ex_Operator(i-1,height)*image1[c][Ex_Sy(height,i-1)][j],

				      Zero_Ex_Operator(j-1,width)*image1[c][i][Ex_Sy(width,j-1)],

				      Zero_Ex_Operator(i-1,height)*Zero_Ex_Operator(j-1,width)
				      *image1[c][Ex_Sy(height,i-1)][Ex_Sy(width,j-1)],

				      Zero_Ex_Operator(i-1,height)*Zero_Ex_Operator(j+1,width)
				      *image1[c][Ex_Sy(height,i-1)][Ex_Sy(width,j+1)],		
		  
				      Zero_Ex_Operator(i-2,height)*image1[c][Ex_Sy(height,i-2)][j],

				      Zero_Ex_Operator(j-2,width)*image1[c][i][Ex_Sy(width,j-2)],

				      Zero_Ex_Operator(i-1,height)*Zero_Ex_Operator(j+2,width)
					  *image1[c][Ex_Sy(height,i-1)][Ex_Sy(width,j+2)]
				      );

	      EncodeSymbol (&rc, &rcm[l], image1[c][i][j]);
	      if (rc.error != RC_OK) break;
	    }
	}
      //Rate Calculation
      i=8*(int)(rc.passed-buf);
      evaluation_rate=(float)(i)/(float)(width*height);
      printf("componet: %d, cumurative =%6lu, current=%6lu, ( bytes ), %f(bpp)\n",c,rc.passed,rc.passed-buf,evaluation_rate);

      buf=rc.passed;  


    }
  //FreeRGBImgMemory(image1,height);
  
  
  /*** Next Bit Plane Processing  6-0  ***/
  //Context_Model Decision
  model_num=2048;
  context_size=11;

  //RANGE_MODEL Active preparation
  for(k=7;k>0;--k)
  {
    for(c=0;c<3;++c)
    {
      //Coding

	  if(k<=4  && c==0)
	    {
	      //RANGE_MODEL Active memory Free
	      FreeFreqTableMemory(ftbl,model_num);
	      free(rcm);

	      model_num/=2;
	      context_size--;

		  ftbl=GetFreqTable(SYMS,model_num);
		  rcm=Get_RC_Model(model_num);

	    }


	  Nth_Bit_Plane_PPM2(image1,ppm,width,height,k-1);//current-bit-plane



	  //Model Initialization
	  for(i=0;i<model_num;++i)
	    {
	      l = ModelInit (&rcm[i], SYMS, ftbl[i], NULL, 2, (Uint)1<<14, ADAPT);
	      if (l != RC_OK) {
		fprintf(stderr, "Error: maxFreq or totFreq > 65536\n");
		exit(1);
	      }
	    }

	  for(i=0;i<height;++i)
	  {
	      for(j=0;j<width;++j)
		{

		  point=Fukugou_Kitaichi_Point_After(ppm[c][i][j],k);
		  l=context_generator_bit(context_size,
/*
| |  | |0
| |  | |
| |X+| |
*/
									(point>>7 & 1),
									(point>>6 & 1),
/*
| | | | |1
| | | | |
| |*|X| |
*/
				      Fukugou_Kitaichi_Point_After(ppm[c][i][Ex_Sy2(width,j-1)],k-(j!=0))>
				      point,
/*
| | | | |2
| | |*| |
| | |X| |
*/				      
				      Fukugou_Kitaichi_Point_After(ppm[c][Ex_Sy2(height,i-1)][j],k-(i!=0))>
				      point,

/*
| | |X|+| |3
| | | | | |
*/
					  Fukugou_Kitaichi_Point_After(ppm[c][i][Ex_Sy2(width,j+1)],k)>
				      point,
/*
| | |X| |4
| | |+| |
| | | | |
*/		  				      
				      Fukugou_Kitaichi_Point_After(ppm[c][Ex_Sy2(height,i+1)][j],k)>
				      point,
/*
| | | | |5
| |*| | |
| | |X| |
*/	 	      
				      Fukugou_Kitaichi_Point_After(ppm[c][Ex_Sy2(height,i-1)][Ex_Sy2(width,j-1)],k-(i!=0 && j!=0))>
				      point,


/*
| | | | |6
| | | |*|
| | |X| |
*/
				      Fukugou_Kitaichi_Point_After(ppm[c][Ex_Sy2(height,i-1)][Ex_Sy2(width,j+1)],k-(i!=0 && j!=0))>
				      point,
/*
| | | | |7
| | | | |
|*| |X| |
*/
				      Fukugou_Kitaichi_Point_After(ppm[c][i][Ex_Sy2(width,j-2)],k-(j!=0))>
				      point,
/*
| | |*| |8
| | | | |
| | |X| |
*/
				      Fukugou_Kitaichi_Point_After(ppm[c][Ex_Sy2(height,i-2)][j],k-(i!=0))>
				      point,
/*
| | | | |9
| | | | |*
| | |X| |
*/
				      Fukugou_Kitaichi_Point_After(ppm[c][Ex_Sy2(height,i-1)][Ex_Sy2(width,j+2)],k-(i!=0))>
				      point
					  );
		  // printf("%d::",l);
		  EncodeSymbol (&rc, &rcm[l], image1[c][i][j]);
		  if (rc.error != RC_OK) break;
		}
	    }



	  //Rate Calculation
	  i=8*(int)(rc.passed-buf);
	  printf("componet: %d, cumurative =%6lu, current=%6lu, ( bytes ), %f(bpp)\n",c,rc.passed,(rc.passed-buf),(float)(i)/(float)(width*height));
	  buf=rc.passed;
	  
	}
    }
  //RANGE_MODEL Active memory Free
  FreeFreqTableMemory(ftbl,model_num);
  free(rcm);

  FinishEncode (&rc);
		
  switch (rc.error) {
  case RC_OK:
    printf("Compressed %lu *3 bytes to %lu bytes\n", (unsigned long)fsize, (unsigned long)rc.passed);

    i=8*(int)rc.passed;

    evaluation_rate=(float)(i)/(float)(width*height*3);

    printf("%s.ppm: 3 component, width(%d)*height(%d):8bit-> %f(bit/pixel,component)\n",filename,width,height,evaluation_rate);

    fflush(stdout);
    break;
  case RC_ERROR:
    fprintf(stderr, "Encode error: Symbol outside range\n");
    break;
  case RC_IO_ERROR:
    fprintf(stderr, "Encode error: Could not output compressed data\n");
  }

  FreeRGBImgMemory(ppm,height);


  return ;
}


void decoding_ppm(char *filename,char *string)
{
  int i,j,k,c;  
  int width,height;
  int point;
  int ***image1,***ppm,***sign_map;

  int model_num,context_size;

  Uint **ftbl;
  rc_model *rcm;

  //From rcdec0.c
  rc_decoder rd;
  FILE *fp;
  Uint fsize, l;

  //Initialization
  if((fp=fopen(filename,"rb"))==NULL)
    {
      printf("Can't Open\n");
      exit(1);
    }
  //setvbuf(fp,ibuf,_IOFBF,IOBUF_SIZE);

  width = (int)getc(fp);  // Get width
  width |= (int)getc(fp) << 8;
  width |= (int)getc(fp) << 16;
  width |= (int)getc(fp) << 24;

  height = (int)getc(fp);  // Get height
  height |= (int)getc(fp) << 8;
  height |= (int)getc(fp) << 16;
  height |= (int)getc(fp) << 24;

  printf("width:%d,height:%d...\n",width,height);//Kakunin

  fsize=(Uint)(width*height*8+1);//Ichiou

  image1=GetRGBImgMemory(width,height);
  //image2=GetRGBImgMemory(width,height);
  ppm=GetRGBImgMemory(width,height);
  sign_map=GetRGBImgMemory(width,height);


  StartDecode (&rd, fp, NULL, 0);
  printf("Decoding...\n");
  fflush(stdout);

#define MODEL_NUM1 2048

  //RANGE_MODEL Active preparation
  ftbl=GetFreqTable(SYMS,MODEL_NUM1) ;
  rcm=Get_RC_Model(MODEL_NUM1);

  model_num=128;
  context_size=7;

  /********   Sign Decoding  ********/
  for(c=0;c<3;c+=2)
  {

      //Probability Model Initialization_1
      for(i=0;i<model_num;++i)
	  {
	    l = ModelInit (&rcm[i], SYMS, ftbl[i], NULL, 2, (Uint)1<<14, ADAPT);
	    if (l != RC_OK) 
	    {
	      fprintf(stderr, "Error: maxFreq or totFreq > 65536\n");
	      exit(1);
	    }
	  }
      //Coding
      for(i=0;i<height;++i)
	  {
	    for(j=0;j<width;++j)
	    {
			//Context_Model_Decision


	        l=context_generator_bit(context_size,
				      Zero_Ex_Operator(i-1,height)*sign_map[c][Ex_Sy(height,i-1)][j],

				      Zero_Ex_Operator(j-1,width)*sign_map[c][i][Ex_Sy(width,j-1)],

				      Zero_Ex_Operator(i-1,height)*Zero_Ex_Operator(j-1,width)
				      *sign_map[c][Ex_Sy(height,i-1)][Ex_Sy(width,j-1)],

				      Zero_Ex_Operator(i-1,height)*Zero_Ex_Operator(j+1,width)
				      *sign_map[c][Ex_Sy(height,i-1)][Ex_Sy(width,j+1)],		
		  
				      Zero_Ex_Operator(i-2,height)*sign_map[c][Ex_Sy(height,i-2)][j],

				      Zero_Ex_Operator(j-2,width)*sign_map[c][i][Ex_Sy(width,j-2)],

				      Zero_Ex_Operator(i-1,height)*Zero_Ex_Operator(j+2,width)
					  *sign_map[c][Ex_Sy(height,i-1)][Ex_Sy(width,j+2)]
				      );

	      sign_map[c][i][j]= DecodeSymbol (&rd, &rcm[l]);

	      if (rd.error != RC_OK) break;
	    }
	  }
  }
  //

  /************Absolute deCoding*************/
  /*** Highest Bit Plane Processing 7  ***/



  for(c=0;c<3;++c)
    {
      //Probability Model Initialization_1
      for(i=0;i<MODEL_NUM1;++i)
	{
	  l = ModelInit (&rcm[i], SYMS, ftbl[i], NULL, 2, (Uint)1<<14, ADAPT);
	  if (l != RC_OK) {
	    fprintf(stderr, "Error: maxFreq or totFreq > 65536\n");
	    exit(1);
	  }
	}

      //Decoding

    for(i=0;i<height;++i)
	{
	  for(j=0;j<width;++j)
	    {
	      //Context_Model_Decision

	      
	      l=context_generator_bit(context_size,
				      Zero_Ex_Operator(i-1,height)*ppm[c][Ex_Sy(height,i-1)][j],

				      Zero_Ex_Operator(j-1,width)*ppm[c][i][Ex_Sy(width,j-1)],

				      Zero_Ex_Operator(i-1,height)*Zero_Ex_Operator(j-1,width)
				      *ppm[c][Ex_Sy(height,i-1)][Ex_Sy(width,j-1)],

				      Zero_Ex_Operator(i-1,height)*Zero_Ex_Operator(j+1,width)
				      *ppm[c][Ex_Sy(height,i-1)][Ex_Sy(width,j+1)],

				      Zero_Ex_Operator(i-2,height)*ppm[c][Ex_Sy(height,i-2)][j],

				      Zero_Ex_Operator(j-2,width)*ppm[c][i][Ex_Sy(width,j-2)],

				      Zero_Ex_Operator(i-1,height)*Zero_Ex_Operator(j+2,width)
					  *ppm[c][Ex_Sy(height,i-1)][Ex_Sy(width,j+2)]
				      );

	      ppm[c][i][j]= DecodeSymbol (&rd, &rcm[l]);

	      if (rd.error != RC_OK) break;
	    }
	}
  }

  //Shift for Context_Model_Decision
  for(c=0;c<3;++c)
  {
	for(i=0;i<height;++i)
    {
		for(j=0;j<width;++j)
		{
 
			 ppm[c][i][j]=ppm[c][i][j]<<7;

		}
    }
  }

  /*** Next Bit Plane Processing  6-0  ***/
#//define MODEL_NUM2 256
  model_num=2048;  
  context_size=11;

  //RANGE_MODEL Active preparation
  ftbl=GetFreqTable(SYMS,model_num) ;
  rcm=Get_RC_Model(model_num);

  for(k=7;k>0;--k)
  {
     for(c=0;c<3;++c)
     {

	  if(k<=4 && c==0)
	    {

	      model_num/=2;
	      context_size--;

	    }



	  //Model Initialization
	  for(i=0;i<model_num;++i)
	    {
	      l = ModelInit (&rcm[i], SYMS, ftbl[i], NULL, 2, (Uint)1<<14, ADAPT);
	      if (l != RC_OK) {
		fprintf(stderr, "Error: maxFreq or totFreq > 65536\n");
		exit(1);
	      }
	    }

	  for(i=0;i<height;++i)
	    {
	      for(j=0;j<width;++j)
		{

		  point=Fukugou_Kitaichi_Point_After(ppm[c][i][j],k);
		  l=context_generator_bit(context_size,
/*
| |  | |0
| |  | |
| |X+| |
*/
									(point>>7 & 1),
									(point>>6 & 1),
/*
| | | | |1
| | | | |
| |*|X| |
*/
				      Fukugou_Kitaichi_Point_After(ppm[c][i][Ex_Sy2(width,j-1)],k-(j!=0))>
				      point,
/*
| | | | |2
| | |*| |
| | |X| |
*/				      
				      Fukugou_Kitaichi_Point_After(ppm[c][Ex_Sy2(height,i-1)][j],k-(i!=0))>
				      point,

/*
| | |X|+| |3
| | | | | |
*/
					  Fukugou_Kitaichi_Point_After(ppm[c][i][Ex_Sy2(width,j+1)],k)>
				      point,
/*
| | |X| |4
| | |+| |
| | | | |
*/		  				      
				      Fukugou_Kitaichi_Point_After(ppm[c][Ex_Sy2(height,i+1)][j],k)>
				      point,
/*
| | | | |5
| |*| | |
| | |X| |
*/	 	      
				      Fukugou_Kitaichi_Point_After(ppm[c][Ex_Sy2(height,i-1)][Ex_Sy2(width,j-1)],k-(i!=0 && j!=0))>
				      point,


/*
| | | | |6
| | | |*|
| | |X| |
*/
				      Fukugou_Kitaichi_Point_After(ppm[c][Ex_Sy2(height,i-1)][Ex_Sy2(width,j+1)],k-(i!=0 && j!=0))>
				      point,
/*
| | | | |7
| | | | |
|*| |X| |
*/
				      Fukugou_Kitaichi_Point_After(ppm[c][i][Ex_Sy2(width,j-2)],k-(j!=0))>
				      point,
/*
| | |*| |8
| | | | |
| | |X| |
*/
				      Fukugou_Kitaichi_Point_After(ppm[c][Ex_Sy2(height,i-2)][j],k-(i!=0))>
				      point,
/*
| | | | |9
| | | | |*
| | |X| |
*/
				      Fukugou_Kitaichi_Point_After(ppm[c][Ex_Sy2(height,i-1)][Ex_Sy2(width,j+2)],k-(i!=0))>
				      point

				       );

		  //image2[i][j]= DecodeSymbol (&rd, &rcm[l]);
		  ppm[c][i][j]+=DecodeSymbol (&rd, &rcm[l])<<(k-1);
		  if (rd.error != RC_OK) break;
		}
	  }
	}
  }


  switch (rd.error) {
  case RC_OK:
    printf("Decompressed %lu bytes to %lu bytes\n", (unsigned long)rd.passed, (unsigned long)fsize);
    fflush(stdout);
    break;
  case RC_ERROR:
    fprintf(stderr, "Decode error: Symbol outside range\n");
    break;
  case RC_IO_ERROR:
    fprintf(stderr, "Decode error: Could not input compressed data\n");
  }

  fclose(fp);

  for(i=0;i<height;++i)
  {
	  for(j=0;j<width;++j)
	  {
		  if(sign_map[0][i][j]==0)
		  {
			  ppm[0][i][j]*=-1;
		  }
		  if(sign_map[2][i][j]==0)
		  {
			  ppm[2][i][j]*=-1;
		  }
	  }
  }

  Inverse_Lif_EMN(ppm,width,height);

  PPMWrite(string,ppm,width,height);
  FreeRGBImgMemory(ppm,height);
  FreeRGBImgMemory(image1,height);
  //FreeRGBImgMemory(image2,height);

  return;
}


int main(int argc, char **argv)
{
  //Gray Scale Image

  if (argc != 4)
  {
	  printf("Usage(Encode): -e imagename.ppm compressed_data_name\n");
	  printf("Usage(Decode): -d compressed_data_name imagename.ppm\n");
	  exit(1);
  }

  if(strcmp(argv[1],"-e") == 0)
  {
	  //Encoding
	  coding_ppm(argv[2],argv[3]);
  }
  else if(strcmp(argv[1],"-d") == 0)
  {
	  //DeCoding
	  decoding_ppm(argv[2],argv[3]);
	  printf("(System time):%u\n",clock());
	  printf("(second):%d\n",clock()/CLOCKS_PER_SEC);
  }
  else
  {
	  printf("Usage(Encode): -e imagename.ppm compressed_data_name\n");
	  printf("Usage(Decode): -d compressed_data_name imagename.ppm\n");
  }


  return 0;
}
