int Zero_Ex_Operator(int ref,int max);

int Ex_Sy(int size,int num);

int Zero_Ex_Operator(int ref,int max)
{
  //int i;//min=0
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

  if(num >= size)
    return (size-1)*2 - num;
  return abs( num );
}

