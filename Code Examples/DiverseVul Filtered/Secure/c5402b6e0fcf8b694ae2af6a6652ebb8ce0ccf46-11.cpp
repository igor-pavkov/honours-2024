static double CubicBC(const double x,const ResizeFilter *resize_filter)
{
  /*
    Cubic Filters using B,C determined values:
       Mitchell-Netravali  B = 1/3 C = 1/3  "Balanced" cubic spline filter
       Catmull-Rom         B = 0   C = 1/2  Interpolatory and exact on linears
       Spline              B = 1   C = 0    B-Spline Gaussian approximation
       Hermite             B = 0   C = 0    B-Spline interpolator

    See paper by Mitchell and Netravali, Reconstruction Filters in Computer
    Graphics Computer Graphics, Volume 22, Number 4, August 1988
    http://www.cs.utexas.edu/users/fussell/courses/cs384g/lectures/mitchell/
    Mitchell.pdf.

    Coefficents are determined from B,C values:
       P0 = (  6 - 2*B       )/6 = coeff[0]
       P1 =         0
       P2 = (-18 +12*B + 6*C )/6 = coeff[1]
       P3 = ( 12 - 9*B - 6*C )/6 = coeff[2]
       Q0 = (      8*B +24*C )/6 = coeff[3]
       Q1 = (    -12*B -48*C )/6 = coeff[4]
       Q2 = (      6*B +30*C )/6 = coeff[5]
       Q3 = (    - 1*B - 6*C )/6 = coeff[6]

    which are used to define the filter:

       P0 + P1*x + P2*x^2 + P3*x^3      0 <= x < 1
       Q0 + Q1*x + Q2*x^2 + Q3*x^3      1 <= x < 2

    which ensures function is continuous in value and derivative (slope).
  */
  if (x < 1.0)
    return(resize_filter->coefficient[0]+x*(x*
      (resize_filter->coefficient[1]+x*resize_filter->coefficient[2])));
  if (x < 2.0)
    return(resize_filter->coefficient[3]+x*(resize_filter->coefficient[4]+x*
      (resize_filter->coefficient[5]+x*resize_filter->coefficient[6])));
  return(0.0);
}