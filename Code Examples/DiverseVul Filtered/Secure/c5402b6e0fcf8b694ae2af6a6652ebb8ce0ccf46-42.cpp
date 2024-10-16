static double CubicSpline(const double x,const ResizeFilter *resize_filter)
{
  if (resize_filter->support <= 2.0)
    {
      /*
        2-lobe Spline filter.
      */
      if (x < 1.0)
        return(((x-9.0/5.0)*x-1.0/5.0)*x+1.0);
      if (x < 2.0)
        return(((-1.0/3.0*(x-1.0)+4.0/5.0)*(x-1.0)-7.0/15.0)*(x-1.0));
      return(0.0);
    }
  if (resize_filter->support <= 3.0)
    {
      /*
        3-lobe Spline filter.
      */
      if (x < 1.0)
        return(((13.0/11.0*x-453.0/209.0)*x-3.0/209.0)*x+1.0);
      if (x < 2.0)
        return(((-6.0/11.0*(x-1.0)+270.0/209.0)*(x-1.0)-156.0/209.0)*(x-1.0));
      if (x < 3.0)
        return(((1.0/11.0*(x-2.0)-45.0/209.0)*(x-2.0)+26.0/209.0)*(x-2.0));
      return(0.0);
    }
  /*
    4-lobe Spline filter.
  */
  if (x < 1.0)
    return(((49.0/41.0*x-6387.0/2911.0)*x-3.0/2911.0)*x+1.0);
  if (x < 2.0)
    return(((-24.0/41.0*(x-1.0)+4032.0/2911.0)*(x-1.0)-2328.0/2911.0)*(x-1.0));
  if (x < 3.0)
    return(((6.0/41.0*(x-2.0)-1008.0/2911.0)*(x-2.0)+582.0/2911.0)*(x-2.0));
  if (x < 4.0)
    return(((-1.0/41.0*(x-3.0)+168.0/2911.0)*(x-3.0)-97.0/2911.0)*(x-3.0));
  return(0.0);
}