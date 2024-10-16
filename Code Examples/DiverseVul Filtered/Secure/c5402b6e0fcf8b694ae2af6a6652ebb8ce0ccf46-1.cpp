static double Lagrange(const double x,const ResizeFilter *resize_filter)
{
  double
    value;

  register ssize_t
    i;

  ssize_t
    n,
    order;

  /*
    Lagrange piecewise polynomial fit of sinc: N is the 'order' of the lagrange
    function and depends on the overall support window size of the filter. That
    is: for a support of 2, it gives a lagrange-4 (piecewise cubic function).

    "n" identifies the piece of the piecewise polynomial.

    See Survey: Interpolation Methods, IEEE Transactions on Medical Imaging,
    Vol 18, No 11, November 1999, p1049-1075, -- Equation 27 on p1064.
  */
  if (x > resize_filter->support)
    return(0.0);
  order=(ssize_t) (2.0*resize_filter->window_support);  /* number of pieces */
  n=(ssize_t) (resize_filter->window_support+x);
  value=1.0f;
  for (i=0; i < order; i++)
    if (i != n)
      value*=(n-i-x)/(n-i);
  return(value);
}