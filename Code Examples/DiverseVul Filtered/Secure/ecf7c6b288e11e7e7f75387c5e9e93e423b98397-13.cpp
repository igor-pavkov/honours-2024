static inline double ConstrainCoordinate(double x)
{
  if (x < (double) -SSIZE_MAX)
    return((double) -SSIZE_MAX);
  if (x > (double) SSIZE_MAX)
    return((double) SSIZE_MAX);
  return(x);
}