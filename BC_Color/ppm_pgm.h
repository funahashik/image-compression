int ***PPMRead(char *filename, int *width, int *height);
void PPMWrite(char *filename, int ***image, int width, int height);
int ***GetRGBImgMemory(int width, int height);
void FreeRGBImgMemory(int ***image, int height);

int **PGMRead(char *filename, int *width, int *height);
void PGMWrite(char *filename, int **image, int width, int height);
int **GetImgMemory(int width, int height);
void FreeImgMemory(int **image, int height);


int ***PPMRead(char *filename, int *width, int *height)
{ 
  // PPMファイルから画素データを読み込む関数
  // filename : 読み込むPPMのファイル名
  // width, height : 画像サイズに書き換えられる。
  //                 呼び出し側では画像サイズを入れたい変数を渡す。
  // 戻り値  : 画素データの入った三次元配列
  //           image[0] : R, image[1] : G, image[2] : B に対応。
  FILE *fp;
  char buf[255];
  int i, j, k, l, x, y;
  int ascii_flag;
  int ***image;
  unsigned char *image_array;

  /* ファイルのオープン */
  if ((fp = fopen(filename, "rb")) == NULL) {
    fprintf(stderr, "int ***PPMRead() : can't open file \"%s\"\n", filename);
    exit(1);
  }
  /* ファイル先頭 1 line 読み込み */
  if (fgets(buf, 255, fp) == NULL) { 
    fprintf(stderr, "int ***PPMRead() : malformed header in ppm file\n");
    exit(1);
  } 
  /* 先頭 1 line で画像形式のチェック */
  if (strncmp(buf, "P3", 2) == 0) {
    // P3 : PPM, ascii形式
    ascii_flag = 1;
  }
  else if(strncmp(buf, "P6", 2) == 0){
    // P6 : PPM, binary形式
    ascii_flag = 0;
  }
  else {
    fprintf(stderr, "int ***PPMRead() : malformed header in ppm file\n");
    exit(1);
  }
  
  do {				/* コメントスキップ */
    if (fgets(buf, 255, fp) == NULL){
      fprintf(stderr, "int ***PPMRead() : malformated header in ppm file...\n");
      exit(1);
    }
  } while(buf[0] == '#');

  /* 画像サイズの読み込み */
  i = 0;
  while (buf[i++] != ' ')
    ;
  x = atoi(buf);
  y = atoi(&(buf[i]));
  /* 階調数の読み飛ばし */
  if (fgets(buf, 255, fp) == NULL){
    fprintf(stderr, "int ***PPMRead() : malformated header in ppm file...\n");
    exit(1);
  }
 
  image = GetRGBImgMemory(x, y);

   /* 画像データの読み込み */
  if(ascii_flag == 1) {  
    for(j = 0; j < y; ++j){
      for(i = 0; i < x; ++i){
	for(k = 0; k < 3; ++k) {
	  if(fscanf(fp, "%d", &image[k][j][i]) == EOF) {  
	    fprintf(stderr, "int ***PPMRead() : can't read PPM format file\n");
	    exit(1);
	  }
	}
      }
    }
  }
  else {
    image_array = (unsigned char*)calloc(x * y * 3, sizeof(unsigned char));
    if (fread(image_array, sizeof(unsigned char), x * y * 3, fp) != x * y * 3) {
      fprintf(stderr, "int ***PPMRead() : can't read PPM format file...\n");
      exit(1);
    }  
    /* RGB値の分離 */
    for(l = 0, j = 0; j < y; ++j) {
      for(i = 0; i < x; ++i) {
	for(k = 0; k < 3; ++k) {
	  image[k][j][i] = image_array[l++];
	}
      }
    }

    free(image_array);
  }
    /* ファイルのクローズ */
  fclose(fp);
  
  *width = x;
  *height = y;
  return image;
}


void PPMWrite(char *filename, int ***image, int width, int height)
{
  FILE *fp;
  int i, j, k, l;
  unsigned char *image_array;

  /* ファイルのオープン */
  if ((fp = fopen(filename, "wb")) == NULL) {
    fprintf(stderr, "void PPMWrite() : can't open file \"%s\"...\n", filename);
    exit(1);
  } 
  image_array = (unsigned char*)calloc(width * height * 3, sizeof(unsigned char));

  /* ヘッダの書き込み */
  fprintf(fp, "P6\n%d %d\n255\n", width, height);
  /* 画像データの書き込み */
  for(l = 0, j = 0; j < height; ++j) {
    for(i = 0; i < width; ++i) {
      for(k = 0; k < 3; ++k) {
	if(image[k][j][i] < 0)
	  image_array[l++] = 0;
	else if(image[k][j][i] > 255)
	  image_array[l++] = 255;
	else 
	  image_array[l++] = (unsigned char)image[k][j][i];
      }
    }
  }
  if (fwrite(image_array, sizeof(unsigned char), width * height * 3, fp)
      != width * height * 3) {
    fprintf(stderr, "void PPMWrite() : can't write PPM format file...\n");
    exit(1);
  }

  free(image_array);
  /* ファイルのクローズ */
  fclose(fp);
  
}


int ***GetRGBImgMemory(int width, int height) {
  // int の三次元配列 image[3][height][width]を返す
  int ***image;
  int j, k;

  image = (int***)calloc(3, sizeof(int**));
  for(k = 0; k < 3; ++k) {
    image[k] = (int**)calloc(height, sizeof(int*));
    for(j = 0; j < height; ++j)
      image[k][j] = (int*)calloc(width, sizeof(int));
  }

  return image;
}


void FreeRGBImgMemory(int ***image, int height) {
  // int の三次元配列 image[3][height][...]を解放
  int j, k;

  for(k = 0; k < 3; ++k) {
    for(j = 0; j < height; ++j)
      free(image[k][j]);
    free(image[k]);
  }
  free(image);
}

int **PGMRead(char *filename, int *width, int *height)
{
  // PGMファイルから画素データを読み込む関数
  // filename : 読み込むPGMのファイル名
  // width, height : 画像サイズに書き換えられる。
  //                 呼び出し側では画像サイズを入れたい変数を渡す。
  // 戻り値  : 画素データの入った二次元配列
  FILE *fp;
  char buf[255];
  int i, j, k, x, y;
  int ascii_flag;
  int **image;
  unsigned char *image_array;
  /* ファイルのオープン */
  if ((fp = fopen(filename, "rb")) == NULL) {
    fprintf(stderr, "int **PGMRead() : can't open file \"%s\"\n", filename);
    exit(1);
  }
  /* ファイル先頭 1 line 読み込み */
  if (fgets(buf, 255, fp) == NULL) {
    fprintf(stderr, "int **PGMRead() : malformed header in pgm file\n");
    exit(1);
  }
  /* 先頭 1 line で画像形式のチェック */
  if (strncmp(buf, "P2", 2) == 0) {
    // P2 : PGM, ascii形式
    ascii_flag = 1;
  }
  else if(strncmp(buf, "P5", 2) == 0){
    // P5 : PGM, binary形式
    ascii_flag = 0;
  }
  else {
    fprintf(stderr, "int **PGMRead() : malformed header in pgm file\n");
    exit(1);
  }

  do {				/* コメントスキップ */
    if (fgets(buf, 255, fp) == NULL){
      fprintf(stderr, "int **PGMRead() : malformed header in pgm file\n");
      exit(1);
    }
  } while(buf[0] == '#');

  /* 画像サイズの読み込み */
  i = 0;
  while(buf[i++] != ' ')
    ;
  x = atoi(buf);
  y = atoi(&(buf[i]));
  /* 階調数の読み飛ばし */
  if (fgets(buf, 255, fp) == NULL){
    fprintf(stderr, "int **PGMRead() : malformed header in pgm file...\n");
    exit(1);
  }

  image = GetImgMemory(x, y);

  /* 画像データの読み込み */
  if(ascii_flag == 1) {
   for(j = 0; j < y; ++j){
      for(i = 0; i < x; ++i){
	if(fscanf(fp, "%d", &image[j][i]) == EOF) {  
	  fprintf(stderr, "int **PGMRead() : can't read PGM format file\n");
	  exit(1);
	}
      }
    }
  }
  else { 
    image_array = (unsigned char*)calloc(x * y, sizeof(unsigned char));
    if(fread(image_array, sizeof(unsigned char), x * y, fp) != x * y) { 
      fprintf(stderr, "int **PGMRead() : can't read PGM format file\n");
      exit(1);
    }
    
    for(j = 0, k = 0; j < y; ++j){
      for(i = 0; i < x; ++i){
        image[j][i] = image_array[k++];
      }
    }  
    free(image_array);

  }
  fclose(fp);


  *width = x;
  *height = y;
  return image;
}

void PGMWrite(char *filename, int **image, int width, int height) {
  FILE *fp;
  int i, j, k;
  unsigned char *image_array;

  /* ファイルのオープン */
  if ((fp = fopen(filename, "wb")) == NULL) {
    fprintf(stderr, "void PGMWrite() : can't open file \"%s\"\n", filename);
    exit(1);
  }
  image_array = (unsigned char*)calloc(width * height, sizeof(unsigned char));
  /* ヘッダの書き込み */
  fprintf(fp, "P5\n%d %d\n255\n", width, height);
  /* 画像データの書き込み */
  for(j = 0, k = 0; j < height; ++j) {
    for(i = 0; i < width; ++i) {
      if(image[j][i] < 0)
	image_array[k++] = 0;
      else if(image[j][i] > 255)
	image_array[k++] = 255;
      else 
	image_array[k++] = (unsigned char)image[j][i];
    }
  }
  if (fwrite(image_array, sizeof(unsigned char), width * height, fp)
      != width * height) {
    fprintf(stderr, "void PGMWrite() : can't write PGM format file...\n");
    exit(1);
  }

  free(image_array);
  /* ファイルのクローズ */
  fclose(fp);

}


int **GetImgMemory(int width, int height) 
{
  // int の二次元配列 image[height][width]を返す
  int **image;
  int j;

  image = (int**)calloc(height, sizeof(int*));
  for(j = 0; j < height; ++j)
    image[j] = (int*)calloc(width, sizeof(int));

  return image;
}

void FreeImgMemory(int **image, int height) {
  // int の二次元配列 image[height][...]を解放
  int j;

  for(j = 0; j < height; ++j)
    free(image[j]);
  free(image);
}
