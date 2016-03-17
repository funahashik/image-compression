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
  // PPM�t�@�C�������f�f�[�^��ǂݍ��ފ֐�
  // filename : �ǂݍ���PPM�̃t�@�C����
  // width, height : �摜�T�C�Y�ɏ�����������B
  //                 �Ăяo�����ł͉摜�T�C�Y����ꂽ���ϐ���n���B
  // �߂�l  : ��f�f�[�^�̓������O�����z��
  //           image[0] : R, image[1] : G, image[2] : B �ɑΉ��B
  FILE *fp;
  char buf[255];
  int i, j, k, l, x, y;
  int ascii_flag;
  int ***image;
  unsigned char *image_array;

  /* �t�@�C���̃I�[�v�� */
  if ((fp = fopen(filename, "rb")) == NULL) {
    fprintf(stderr, "int ***PPMRead() : can't open file \"%s\"\n", filename);
    exit(1);
  }
  /* �t�@�C���擪 1 line �ǂݍ��� */
  if (fgets(buf, 255, fp) == NULL) { 
    fprintf(stderr, "int ***PPMRead() : malformed header in ppm file\n");
    exit(1);
  } 
  /* �擪 1 line �ŉ摜�`���̃`�F�b�N */
  if (strncmp(buf, "P3", 2) == 0) {
    // P3 : PPM, ascii�`��
    ascii_flag = 1;
  }
  else if(strncmp(buf, "P6", 2) == 0){
    // P6 : PPM, binary�`��
    ascii_flag = 0;
  }
  else {
    fprintf(stderr, "int ***PPMRead() : malformed header in ppm file\n");
    exit(1);
  }
  
  do {				/* �R�����g�X�L�b�v */
    if (fgets(buf, 255, fp) == NULL){
      fprintf(stderr, "int ***PPMRead() : malformated header in ppm file...\n");
      exit(1);
    }
  } while(buf[0] == '#');

  /* �摜�T�C�Y�̓ǂݍ��� */
  i = 0;
  while (buf[i++] != ' ')
    ;
  x = atoi(buf);
  y = atoi(&(buf[i]));
  /* �K�����̓ǂݔ�΂� */
  if (fgets(buf, 255, fp) == NULL){
    fprintf(stderr, "int ***PPMRead() : malformated header in ppm file...\n");
    exit(1);
  }
 
  image = GetRGBImgMemory(x, y);

   /* �摜�f�[�^�̓ǂݍ��� */
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
    /* RGB�l�̕��� */
    for(l = 0, j = 0; j < y; ++j) {
      for(i = 0; i < x; ++i) {
	for(k = 0; k < 3; ++k) {
	  image[k][j][i] = image_array[l++];
	}
      }
    }

    free(image_array);
  }
    /* �t�@�C���̃N���[�Y */
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

  /* �t�@�C���̃I�[�v�� */
  if ((fp = fopen(filename, "wb")) == NULL) {
    fprintf(stderr, "void PPMWrite() : can't open file \"%s\"...\n", filename);
    exit(1);
  } 
  image_array = (unsigned char*)calloc(width * height * 3, sizeof(unsigned char));

  /* �w�b�_�̏������� */
  fprintf(fp, "P6\n%d %d\n255\n", width, height);
  /* �摜�f�[�^�̏������� */
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
  /* �t�@�C���̃N���[�Y */
  fclose(fp);
  
}


int ***GetRGBImgMemory(int width, int height) {
  // int �̎O�����z�� image[3][height][width]��Ԃ�
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
  // int �̎O�����z�� image[3][height][...]�����
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
  // PGM�t�@�C�������f�f�[�^��ǂݍ��ފ֐�
  // filename : �ǂݍ���PGM�̃t�@�C����
  // width, height : �摜�T�C�Y�ɏ�����������B
  //                 �Ăяo�����ł͉摜�T�C�Y����ꂽ���ϐ���n���B
  // �߂�l  : ��f�f�[�^�̓������񎟌��z��
  FILE *fp;
  char buf[255];
  int i, j, k, x, y;
  int ascii_flag;
  int **image;
  unsigned char *image_array;
  /* �t�@�C���̃I�[�v�� */
  if ((fp = fopen(filename, "rb")) == NULL) {
    fprintf(stderr, "int **PGMRead() : can't open file \"%s\"\n", filename);
    exit(1);
  }
  /* �t�@�C���擪 1 line �ǂݍ��� */
  if (fgets(buf, 255, fp) == NULL) {
    fprintf(stderr, "int **PGMRead() : malformed header in pgm file\n");
    exit(1);
  }
  /* �擪 1 line �ŉ摜�`���̃`�F�b�N */
  if (strncmp(buf, "P2", 2) == 0) {
    // P2 : PGM, ascii�`��
    ascii_flag = 1;
  }
  else if(strncmp(buf, "P5", 2) == 0){
    // P5 : PGM, binary�`��
    ascii_flag = 0;
  }
  else {
    fprintf(stderr, "int **PGMRead() : malformed header in pgm file\n");
    exit(1);
  }

  do {				/* �R�����g�X�L�b�v */
    if (fgets(buf, 255, fp) == NULL){
      fprintf(stderr, "int **PGMRead() : malformed header in pgm file\n");
      exit(1);
    }
  } while(buf[0] == '#');

  /* �摜�T�C�Y�̓ǂݍ��� */
  i = 0;
  while(buf[i++] != ' ')
    ;
  x = atoi(buf);
  y = atoi(&(buf[i]));
  /* �K�����̓ǂݔ�΂� */
  if (fgets(buf, 255, fp) == NULL){
    fprintf(stderr, "int **PGMRead() : malformed header in pgm file...\n");
    exit(1);
  }

  image = GetImgMemory(x, y);

  /* �摜�f�[�^�̓ǂݍ��� */
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

  /* �t�@�C���̃I�[�v�� */
  if ((fp = fopen(filename, "wb")) == NULL) {
    fprintf(stderr, "void PGMWrite() : can't open file \"%s\"\n", filename);
    exit(1);
  }
  image_array = (unsigned char*)calloc(width * height, sizeof(unsigned char));
  /* �w�b�_�̏������� */
  fprintf(fp, "P5\n%d %d\n255\n", width, height);
  /* �摜�f�[�^�̏������� */
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
  /* �t�@�C���̃N���[�Y */
  fclose(fp);

}


int **GetImgMemory(int width, int height) 
{
  // int �̓񎟌��z�� image[height][width]��Ԃ�
  int **image;
  int j;

  image = (int**)calloc(height, sizeof(int*));
  for(j = 0; j < height; ++j)
    image[j] = (int*)calloc(width, sizeof(int));

  return image;
}

void FreeImgMemory(int **image, int height) {
  // int �̓񎟌��z�� image[height][...]�����
  int j;

  for(j = 0; j < height; ++j)
    free(image[j]);
  free(image);
}
