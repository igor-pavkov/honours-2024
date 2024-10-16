static double Kaiser(const double x,const ResizeFilter *resize_filter)
{
  /*
    Kaiser Windowing Function (bessel windowing)

       I0( beta * sqrt( 1-x^2) ) / IO(0)

    Beta (coeff[0]) is a free value from 5 to 8 (defaults to 6.5).
    However it is typically defined in terms of Alpha*PI

    The normalization factor (coeff[1]) is not actually needed,
    but without it the filters has a large value at x=0 making it
    difficult to compare the function with other windowing functions.
  */
  return(resize_filter->coefficient[1]*I0(resize_filter->coefficient[0]*
    sqrt((double) (1.0-x*x))));
}