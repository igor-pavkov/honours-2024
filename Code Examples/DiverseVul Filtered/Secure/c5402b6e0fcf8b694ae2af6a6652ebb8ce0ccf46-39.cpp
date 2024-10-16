static double I0(double x)
{
  double
    sum,
    t,
    y;

  register ssize_t
    i;

  /*
    Zeroth order Bessel function of the first kind.
  */
  sum=1.0;
  y=x*x/4.0;
  t=y;
  for (i=2; t > MagickEpsilon; i++)
  {
    sum+=t;
    t*=y/((double) i*i);
  }
  return(sum);
}