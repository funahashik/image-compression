//
//This Source Code is reported follwing conferece and paper(Gray-Scale Image Version)
//K.Kikuchi, K. Funahashi, and S.Muramatsu, Simple Bit-plane Coding for Lossless Image Compression and Extended FUnctionalities, 27th PCS(PCS 2009)
//K.Funahashi, K.Kikuchi, and S.Muramatsu, Progressive Bitplane Coding for Lossless Image Compression  [in Japanese], Technical report of IEICE. VLD 108(106), 23-28, 2008-06-19
//K.Kikuchi, R.Abe, and S.Muramatsu, Simple Bitplane Coding and Its Application to Multi-Functional Image Compression, EICE Transactions on Fundamentals of Electronics, Communications and Computer Sciences
//
/*Context Model Only Most Upper Plane*/
/*Not Use Filterling Method for decision context model*/
//Context-Changing


#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>

#include<math.h>
#include<string.h>
#include<time.h>

#include"range.h"

#include"ppm_pgm.h"

//From rcenc0.c
#define SYMS 2

rc_model *Get_RC_Model(int Model_Num)
{
  rc_model *rcm;

  rcm=(rc_model*)calloc(Model_Num, sizeof(rc_model));
  return rcm;
}

Uint **GetFreqTable(int Symbol_Num,int Model_Num) 
{
  Uint **ftbl;
  int j;

  ftbl = (Uint**)calloc(Model_Num, sizeof(Uint*));
  for(j = 0; j < Model_Num; ++j)
    ftbl[j] = (Uint*)calloc(Symbol_Num, sizeof(Uint));

  return ftbl;
}

void FreeFreqTableMemory(Uint **ftbl, int Model_Num) {
  int j;

  for(j = 0; j < Model_Num; ++j)
    free(ftbl[j]);
  free(ftbl);
}

int context_modeling(int context_size,...)
{
  va_list args;
  int context=0,i;

  if(context_size<1) return 0;

  va_start(args,context_size);

  for(i=0;i<context_size;i++)
    {
      context+=va_arg(args,int)<< i;
    }

  return context;
}

void GetNthBitPlane(int **after,int **before,int width, int height,int k)
{

  int i,j;
  int saguri=1;

  saguri=saguri << k;

  for(i=0;i<height;++i)
  {
    for(j=0;j<width;++j)
	{
	  //f=input_image[i][j];

	  after[i][j]=before[i][j] & saguri;

	  after[i][j]=after[i][j]>>k;

	}
  }
  return;
}

int Expectation_Value(int point,int k)
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

int Zero_Ex_Operator(int ref,int max)
{
//  int i;//min=0
  if(ref>max-1 || ref<0)
    {
      return 0;
    }
  else
    {
      return 1;
    }
}

int Ex_Sy(int size,int num)
{
  // For WS,WA
  // num :refer_point
  // size:(size = width or height)

  //dcb|abcd|cba

  if(num >= size)
  {
	  //x-2(x-(size-1))=2(w-1)-x
    return (size-1)*2 - num;
  }  	
  return abs(num);
}

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

void coding_pgm(char *in_filename, char *out_filename)
{
  int i,j,k;
  int width,height;
  int **origin,**image1;
  int point;

  int poor,rich;

  int model_num,context_size;

  clock_t t1,t2;

  Uint l,buf1,buf2;

  Uint **ftbl;
  rc_model *rcm;
  Uint *model_count;

  //From rcenc0.c
  rc_encoder rc;
  FILE *fp;//For_Writing

 //Encoded Image Decision
  origin=PGMRead(in_filename, &width, &height);
  image1=GetImgMemory(width,height);
  t1=clock();

 //Writing_File_Name Decision
  if((fp=fopen(out_filename,"wb"))==NULL)
  {
	  printf("Can't Open\n");
      exit(1);
  }

  //Initialization Writing
  printf("\nInput  file: %s\nOutput file: %s\n\nBC header information\n  width  =%10d\n  height =%10d\n\n",in_filename,out_filename,width,height);
  putc(width & 0xff, fp);  //Save width
  putc((width >> 8) & 0xff, fp);
  putc((width >> 16) & 0xff, fp);
  putc((width >> 24) & 0xff, fp);
  
  putc(height & 0xff, fp);  //Save height 
  putc((height >> 8) & 0xff, fp);
  putc((height >> 16) & 0xff, fp);
  putc((height >> 24) & 0xff, fp);


  StartEncode (&rc, fp, NULL, 0);
  fflush(stdout);

  /*** MSB Coding  ***/

  //Processing Image Decision
  GetNthBitPlane(image1,origin,width,height,7);

#define MODEL_NUM1 2048

  //RANGE_MODEL Active preparation
  ftbl=GetFreqTable(SYMS,MODEL_NUM1) ;
  rcm=Get_RC_Model(MODEL_NUM1);
  model_count=(Uint*)calloc(MODEL_NUM1, sizeof(Uint));

#undef MODEL_NUM1
  model_num=128;
  context_size=7;

  //Probability Model Initialization_1
  for(i=0;i<model_num;++i)
    {
      l = ModelInit (&rcm[i], SYMS, ftbl[i], NULL, 2, (Uint)1<<14, ADAPT);
      if (l != RC_OK) {
	fprintf(stderr, "Error: maxFreq or totFreq > 65536\n");
	exit(1);
      }
	  model_count[i]=0;//model_count Initialization
    }

  //Coding
  for(i=0;i<height;++i)
  {
      for(j=0;j<width;++j)
	{
	  //Context_Model_Decision

	  l=context_modeling(context_size,
				  Zero_Ex_Operator(i-1,height)*image1[Ex_Sy(height,i-1)][j],
				  Zero_Ex_Operator(j-1,width)*image1[i][Ex_Sy(width,j-1)],
				  Zero_Ex_Operator(i-1,height)*Zero_Ex_Operator(j-1,width)*image1[Ex_Sy(height,i-1)][Ex_Sy(width,j-1)],
				  Zero_Ex_Operator(i-1,height)*Zero_Ex_Operator(j+1,width)*image1[Ex_Sy(height,i-1)][Ex_Sy(width,j+1)],				  
				  Zero_Ex_Operator(i-2,height)*image1[Ex_Sy(height,i-2)][j],
				  Zero_Ex_Operator(j-2,width)*image1[i][Ex_Sy(width,j-2)],
				  Zero_Ex_Operator(i-1,height)*Zero_Ex_Operator(j+2,width)*image1[Ex_Sy(height,i-1)][Ex_Sy(width,j+2)]
				  );

	  EncodeSymbol (&rc, &rcm[l], image1[i][j]);
	  model_count[l]++;
	  if (rc.error != RC_OK) break;
	}
  }

  //Poor-Rich
  poor=0;
  rich=0;
  for(i=0;i<model_num;++i)
  {
	  if(model_count[i]>=64)
	  {
		  rich++;
	  }
	  if(model_count[i]<=63 && model_count[i]>=1)
	  {
		  poor++;
	  }
  }
  printf("poor:%d, rich:%d\n",poor,rich);

  //Rate Calculation
  printf("Bitplane:rc.passed: 8\n  total byte     =%lu(bytes)\n  total bit rate =%Lf(bpp)\n",rc.passed,8.0*(double)(rc.passed)/(double)(width*height));
  printf("Bitplane:ftell(fp): 8\n  total byte     =%lu(bytes)\n  total bit rate =%Lf(bpp)\n\n",ftell(fp),8.0*(double)(ftell(fp))/(double)(width*height));
  buf1=rc.passed;
  buf2=(unsigned int)ftell(fp);



  /*** Next Bit Plane Processing  6-0  ***/
  model_num=2048;
  context_size=11;
  
  for(k=7;k>0;--k)
    {
      
      if(k<=4)
      {
	     model_num/=2;
	     context_size--;
      }
      
      GetNthBitPlane(image1,origin,width,height,k-1);
      //Model Initialization
      for(i=0;i<model_num;++i)
	  {
		l = ModelInit (&rcm[i], SYMS, ftbl[i], NULL, 2, (Uint)1<<14, ADAPT);
	    if (l != RC_OK) 
		{
			fprintf(stderr, "Error: maxFreq or totFreq > 65536\n");
			exit(1);
		}
	    model_count[i]=0;//model_count Initialization
	  }



      for(i=0;i<height;++i)
	  {
		  for(j=0;j<width;++j)
		  {
			point=Expectation_Value(origin[i][j],k);
			 l=context_modeling(context_size,
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
				      Expectation_Value(origin[i][Ex_Sy2(width,j-1)],k-(j!=0))>
				      point,
/*
| | | | |2
| | |*| |
| | |X| |
*/				      
				      Expectation_Value(origin[Ex_Sy2(height,i-1)][j],k-(i!=0))>
				      point,

/*
| | |X|+| |3
| | | | | |
*/
					  Expectation_Value(origin[i][Ex_Sy2(width,j+1)],k)>
				      point,
/*
| | |X| |4
| | |+| |
| | | | |
*/		  				      
				      Expectation_Value(origin[Ex_Sy2(height,i+1)][j],k)>
				      point,
/*
| | | | |5
| |*| | |
| | |X| |
*/	 	      
				      Expectation_Value(origin[Ex_Sy2(height,i-1)][Ex_Sy2(width,j-1)],k-(i!=0 && j!=0))>
				      point,


/*
| | | | |6
| | | |*|
| | |X| |
*/
				      Expectation_Value(origin[Ex_Sy2(height,i-1)][Ex_Sy2(width,j+1)],k-(i!=0 && j!=0))>
				      point,
/*
| | | | |7
| | | | |
|*| |X| |
*/
				      Expectation_Value(origin[i][Ex_Sy2(width,j-2)],k-(j!=0))>
				      point,
/*
| | |*| |8
| | | | |
| | |X| |
*/
				      Expectation_Value(origin[Ex_Sy2(height,i-2)][j],k-(i!=0))>
				      point,
/*
| | | | |9
| | | | |*
| | |X| |
*/
				      Expectation_Value(origin[Ex_Sy2(height,i-1)][Ex_Sy2(width,j+2)],k-(i!=0))>
				      point

					       );

			    EncodeSymbol (&rc, &rcm[l], image1[i][j]);
				model_count[l]++;
			    if (rc.error != RC_OK) break;
			  }



		}

	  //Poor-Rich
	  poor=0;
	  rich=0;
	  for(i=0;i<model_num;++i)
	  {
		  if(model_count[i]>=64)
		  {
			  rich++;
		  }
		  if(model_count[i]<=63 && model_count[i]>=1)
		  {
			  poor++;
		  }
	  }
	  printf("poor:%d, rich:%d\n",poor,rich);

	  //Rate Calc
	  printf("Bitplane:rc.passed: %d\n  total bytes               =%lu(bytes)\n  total bit rate            =%Lf(bpp)\n  current plane's bytes     =%lu(byte)\n  current plane's bit rate  =%f(bpp)\n",k,rc.passed,(double)(8*rc.passed)/(double)(width*height),(rc.passed-buf1),8.0*(double)(rc.passed-buf1)/(double)(width*height));
	  printf("Bitplane:ftell(fp): %d\n  total bytes               =%lu(bytes)\n  total bit rate            =%Lf(bpp)\n  current plane's bytes     =%lu(byte)\n  current plane's bit rate  =%f(bpp)\n\n",k,ftell(fp),(double)(8*ftell(fp))/(double)(width*height),(ftell(fp)-buf2),8.0*(double)(ftell(fp)-buf2)/(double)(width*height));


	  buf1=rc.passed;
	  buf2=(unsigned int)ftell(fp);
  }

  FinishEncode (&rc);
		
  //time(&t2);
  t2=clock();
  //t2=gettimeofday_sec();

  switch (rc.error) {
  case RC_OK:
	  printf("Total Bit Rate:rc.passed       : %f(bit/pixel)\n",(float)(8*rc.passed)/(float)(width*height));
	  printf("Total Bytes   :rc.passed      : %lu(bytes)\n\n",rc.passed);
	  printf("Total Bit Rate:ftell(fp)      : %f(bit/pixel)\n",(float)(8*ftell(fp))/(float)(width*height));
	  printf("Total Bytes   :ftell(fp)      : %lu(bytes)\n\n",ftell(fp));
	  printf("Total Encoding Time           : %f(sec)\n\n",(double)(t2-t1)/(double)CLOCKS_PER_SEC);
	  printf("%s : %d (width) x %d (height) : %f bpp\n",in_filename,width,height,(float)(8*rc.passed)/(float)(width*height));

    fflush(stdout);
    break;
  case RC_ERROR:
    fprintf(stderr, "Encode error: Symbol outside range\n");
    break;
  case RC_IO_ERROR:
    fprintf(stderr, "Encode error: Could not output compressed data\n");
  }

  FreeImgMemory(origin,height);
  FreeImgMemory(image1,height);


  return ;
}

void decoding_pgm(char *in_filename,char *out_filename)
{
  int i,j,k;  
  int width,height;
  int point;
  int **pgm;

  int model_num,context_size;

  Uint **ftbl;
  rc_model *rcm;
  clock_t t1,t2;

  //From rcdec0.c
  rc_decoder rd;
  FILE *fp;
  Uint fsize, l;
  t1=clock();
  //Initialization
  if((fp=fopen(in_filename,"rb"))==NULL)
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

  fsize=(Uint)(width*height*8);//Ichiou

  pgm=GetImgMemory(width,height);

  StartDecode (&rd, fp, NULL, 0);

  fflush(stdout);

  //

  /*** Highest Bit Plane Processing 7  ***/
#define MODEL_NUM1 2048

  //RANGE_MODEL Active preparation
  ftbl=GetFreqTable(SYMS,MODEL_NUM1) ;
  rcm=Get_RC_Model(MODEL_NUM1);

  model_num=128;
  context_size=7;

  //Probability Model Initialization_1
  for(i=0;i<model_num;++i)
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

	  l=context_modeling(context_size,
				  Zero_Ex_Operator(i-1,height)*pgm[Ex_Sy(height,i-1)][j],
				  Zero_Ex_Operator(j-1,width)*pgm[i][Ex_Sy(width,j-1)],
				  Zero_Ex_Operator(i-1,height)*Zero_Ex_Operator(j-1,width)*pgm[Ex_Sy(height,i-1)][Ex_Sy(width,j-1)],
				  Zero_Ex_Operator(i-1,height)*Zero_Ex_Operator(j+1,width)*pgm[Ex_Sy(height,i-1)][Ex_Sy(width,j+1)],
				  Zero_Ex_Operator(i-2,height)*pgm[Ex_Sy(height,i-2)][j],
				  Zero_Ex_Operator(j-2,width)*pgm[i][Ex_Sy(width,j-2)],
				  Zero_Ex_Operator(i-1,height)*Zero_Ex_Operator(j+2,width)*pgm[Ex_Sy(height,i-1)][Ex_Sy(width,j+2)]
				   );

	  pgm[i][j]= DecodeSymbol (&rd, &rcm[l]);

	  if (rd.error != RC_OK) break;

	}
  }

  //Shift for Context_Model_Decision
  for(i=0;i<height;++i)
  {
      for(j=0;j<width;++j)
	{
 
	  pgm[i][j]=pgm[i][j]<<7;

	}
  }

#undef MODEL_NUM1
  /*** Next Bit Plane Processing  6-0  ***/
  model_num=2048;
  context_size=11;

  for(k=7;k>0;--k)
    {
      
	  //Model Change
      if(k<=4)
	  {
	  model_num/=2;
	  context_size--;
	  }

      //Model Initialization
      for(i=0;i<model_num;++i)
   	  {
		  l = ModelInit (&rcm[i], SYMS, ftbl[i], NULL, 2, (Uint)1<<14, ADAPT);
	      if (l != RC_OK) 
		  {
			  fprintf(stderr, "Error: maxFreq or totFreq > 65536\n");
	          exit(1);
	      }
	  }

      for(i=0;i<height;++i)
	  {
	      for(j=0;j<width;++j)
	      {
	         point=Expectation_Value(pgm[i][j],k);
	         l=context_modeling(context_size,


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
				      Expectation_Value(pgm[i][Ex_Sy2(width,j-1)],k-(j!=0))>
				      point,
/*
| | | | |2
| | |*| |
| | |X| |
*/				      
				      Expectation_Value(pgm[Ex_Sy2(height,i-1)][j],k-(i!=0))>
				      point,

/*
| | |X|+| |3
| | | | | |
*/
					  Expectation_Value(pgm[i][Ex_Sy2(width,j+1)],k)>
				      point,
/*
| | |X| |4
| | |+| |
| | | | |
*/		  				      
				      Expectation_Value(pgm[Ex_Sy2(height,i+1)][j],k)>
				      point,
/*
| | | | |5
| |*| | |
| | |X| |
*/	 	      
				      Expectation_Value(pgm[Ex_Sy2(height,i-1)][Ex_Sy2(width,j-1)],k-(i!=0 && j!=0))>
				      point,


/*
| | | | |6
| | | |*|
| | |X| |
*/
				      Expectation_Value(pgm[Ex_Sy2(height,i-1)][Ex_Sy2(width,j+1)],k-(i!=0 && j!=0))>
				      point,
/*
| | | | |7
| | | | |
|*| |X| |
*/
				      Expectation_Value(pgm[i][Ex_Sy2(width,j-2)],k-(j!=0))>
				      point,
/*
| | |*| |8
| | | | |
| | |X| |
*/
				      Expectation_Value(pgm[Ex_Sy2(height,i-2)][j],k-(i!=0))>
				      point,
/*
| | | | |9
| | | | |*
| | |X| |
*/
				      Expectation_Value(pgm[Ex_Sy2(height,i-1)][Ex_Sy2(width,j+2)],k-(i!=0))>
				      point
				      )
				       ;

	      //image2[i][j]= DecodeSymbol (&rd, &rcm[l]);
	      pgm[i][j]+=DecodeSymbol (&rd, &rcm[l])<<(k-1);
	      if (rd.error != RC_OK) break;
	    }
	}

  }
  t2=clock();

  switch (rd.error) {
  case RC_OK:
    printf("Input File : %s\nOutput File :%s\n\n",in_filename,out_filename);
	printf("width:%d,height:%d\n\n",width,height);//Kakunin
    printf("Total Decoding Time           : %f(sec)\n\n",(double)(t2-t1)/(double)CLOCKS_PER_SEC);

    fflush(stdout);
    break;
  case RC_ERROR:
    fprintf(stderr, "Decode error: Symbol outside range\n");
    break;
  case RC_IO_ERROR:
    fprintf(stderr, "Decode error: Could not input compressed data\n");
  }

  fclose(fp);


  PGMWrite(out_filename,pgm,width,height);
  FreeImgMemory(pgm,height);

  return;
}

int main(int argc, char **argv)
{
  //Gray Scale Image

  if (argc != 4)
  {
	  printf("Usage(Encode): -e imagename.pgm compressed_data_name\n");
	  printf("Usage(Decode): -d compressed_data_name imagename.pgm\n");
	  exit(1);
  }

  if(strcmp(argv[1],"-e") == 0)
  {
	  //Encoding
	  coding_pgm(argv[2],argv[3]);
  }
  else if(strcmp(argv[1],"-d") == 0)
  {
	  //DeCoding
	  decoding_pgm(argv[2],argv[3]);
  }
  else
  {
	  printf("Usage(Encode): -e imagename.pgm compressed_data_name\n");
	  printf("Usage(Decode): -d compressed_data_name imagename.pgm\n");
  }

  return 0;
}
