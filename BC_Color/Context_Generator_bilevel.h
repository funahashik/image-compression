int context_generator_bit_tasuuketsu(int context_size,...);

int context_generator_bit(int context_size,...);
int compared(int point,int compared);

int context_generator_bit_tasuuketsu(int context_size,...)
{
  va_list args;
  int context=0,i;

  if(context_size<1) return 0;

  va_start(args,context_size);

  for(i=0;i<context_size;i++)
    {
      context=context+va_arg(args,int);
    }

  return context;
}

int context_generator_bit(int context_size,...)
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

int compared(int point,int compared)
{
  //int q;

  if(point<compared)
    {
      return 0;
    }
  return 1;
}
