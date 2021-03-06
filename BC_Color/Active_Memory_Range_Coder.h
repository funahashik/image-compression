rc_model *Get_RC_Model(int Model_Num)
{
  rc_model *rcm;

  rcm=(rc_model*)calloc(Model_Num, sizeof(rc_model));
  return rcm;
}

Uint **GetFreqTable(int Symbol_Num,int Model_Num) 
{
  // int の二次元配��Eimage[height][width]を返す
  Uint **ftbl;
  int j;

  ftbl = (Uint**)calloc(Model_Num, sizeof(Uint*));
  for(j = 0; j < Model_Num; ++j)
    ftbl[j] = (Uint*)calloc(Symbol_Num, sizeof(Uint));

  return ftbl;
}

void FreeFreqTableMemory(Uint **ftbl, int Model_Num) {
  // int の二次元配��Eimage[height][...]を解��E  
  int j;

  for(j = 0; j < Model_Num; ++j)
    free(ftbl[j]);
  free(ftbl);
}
