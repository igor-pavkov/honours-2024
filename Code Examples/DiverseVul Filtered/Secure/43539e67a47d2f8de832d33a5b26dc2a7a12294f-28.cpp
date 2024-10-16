static inline unsigned int Hq2XPatternToNumber(const int *pattern)
{
  ssize_t
    i;

  unsigned int
    result,
    order;

  result=0;
  order=1;
  for (i=7; i >= 0; i--)
  {
    result+=order*pattern[i];
    order*=2;
  }
  return(result);
}