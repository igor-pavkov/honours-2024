static double Gaussian(const double x,const ResizeFilter *resize_filter)
{
  /*
    Gaussian with a sigma = 1/2 (or as user specified)

    Gaussian Formula (1D) ...
        exp( -(x^2)/((2.0*sigma^2) ) / (sqrt(2*PI)*sigma^2))

    Gaussian Formula (2D) ...
        exp( -(x^2+y^2)/(2.0*sigma^2) ) / (PI*sigma^2) )
    or for radius
        exp( -(r^2)/(2.0*sigma^2) ) / (PI*sigma^2) )

    Note that it is only a change from 1-d to radial form is in the
    normalization multiplier which is not needed or used when Gaussian is used
    as a filter.

    The constants are pre-calculated...

        coeff[0]=sigma;
        coeff[1]=1.0/(2.0*sigma^2);
        coeff[2]=1.0/(sqrt(2*PI)*sigma^2);

        exp( -coeff[1]*(x^2)) ) * coeff[2];

    However the multiplier coeff[1] is need, the others are informative only.

    This separates the gaussian 'sigma' value from the 'blur/support'
    settings allowing for its use in special 'small sigma' gaussians,
    without the filter 'missing' pixels because the support becomes too
    small.
  */
  return(exp((double)(-resize_filter->coefficient[1]*x*x)));
}