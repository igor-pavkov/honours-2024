static void FixLogical(unsigned char *Buff,int ldblk)
{
unsigned char mask=128;
unsigned char *BuffL = Buff;
unsigned char val = 0;

  while(ldblk-->0)
  {
    if(*Buff++ != 0)
      val |= mask;

    mask >>= 1;
    if(mask==0)
    {
      *BuffL++ = val;
      val = 0;
      mask = 128;
    }

  }
  *BuffL = val;
}