static int QuantizeErrorCompare(const void *error_p,const void *error_q)
{
  double
    *p,
    *q;

  p=(double *) error_p;
  q=(double *) error_q;
  if (*p > *q)
    return(1);
  if (fabs(*q-*p) <= MagickEpsilon)
    return(0);
  return(-1);
}