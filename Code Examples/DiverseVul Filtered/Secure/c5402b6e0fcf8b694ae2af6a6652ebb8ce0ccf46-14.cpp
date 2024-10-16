static double SincFast(const double x,
  const ResizeFilter *magick_unused(resize_filter))
{
  magick_unreferenced(resize_filter);

  /*
    Approximations of the sinc function sin(pi x)/(pi x) over the interval
    [-4,4] constructed by Nicolas Robidoux and Chantal Racette with funding
    from the Natural Sciences and Engineering Research Council of Canada.

    Although the approximations are polynomials (for low order of
    approximation) and quotients of polynomials (for higher order of
    approximation) and consequently are similar in form to Taylor polynomials /
    Pade approximants, the approximations are computed with a completely
    different technique.

    Summary: These approximations are "the best" in terms of bang (accuracy)
    for the buck (flops). More specifically: Among the polynomial quotients
    that can be computed using a fixed number of flops (with a given "+ - * /
    budget"), the chosen polynomial quotient is the one closest to the
    approximated function with respect to maximum absolute relative error over
    the given interval.

    The Remez algorithm, as implemented in the boost library's minimax package,
    is the key to the construction: http://www.boost.org/doc/libs/1_36_0/libs/
    math/doc/sf_and_dist/html/math_toolkit/backgrounders/remez.html

    If outside of the interval of approximation, use the standard trig formula.
  */
  if (x > 4.0)
    {
      const double alpha=(double) (MagickPI*x);
      return(sin((double) alpha)/alpha);
    }
  {
    /*
      The approximations only depend on x^2 (sinc is an even function).
    */
    const double xx = x*x;
#if MAGICKCORE_QUANTUM_DEPTH <= 8
    /*
      Maximum absolute relative error 6.3e-6 < 1/2^17.
    */
    const double c0 = 0.173610016489197553621906385078711564924e-2L;
    const double c1 = -0.384186115075660162081071290162149315834e-3L;
    const double c2 = 0.393684603287860108352720146121813443561e-4L;
    const double c3 = -0.248947210682259168029030370205389323899e-5L;
    const double c4 = 0.107791837839662283066379987646635416692e-6L;
    const double c5 = -0.324874073895735800961260474028013982211e-8L;
    const double c6 = 0.628155216606695311524920882748052490116e-10L;
    const double c7 = -0.586110644039348333520104379959307242711e-12L;
    const double p =
      c0+xx*(c1+xx*(c2+xx*(c3+xx*(c4+xx*(c5+xx*(c6+xx*c7))))));
    return((xx-1.0)*(xx-4.0)*(xx-9.0)*(xx-16.0)*p);
#elif MAGICKCORE_QUANTUM_DEPTH <= 16
    /*
      Max. abs. rel. error 2.2e-8 < 1/2^25.
    */
    const double c0 = 0.173611107357320220183368594093166520811e-2L;
    const double c1 = -0.384240921114946632192116762889211361285e-3L;
    const double c2 = 0.394201182359318128221229891724947048771e-4L;
    const double c3 = -0.250963301609117217660068889165550534856e-5L;
    const double c4 = 0.111902032818095784414237782071368805120e-6L;
    const double c5 = -0.372895101408779549368465614321137048875e-8L;
    const double c6 = 0.957694196677572570319816780188718518330e-10L;
    const double c7 = -0.187208577776590710853865174371617338991e-11L;
    const double c8 = 0.253524321426864752676094495396308636823e-13L;
    const double c9 = -0.177084805010701112639035485248501049364e-15L;
    const double p =
      c0+xx*(c1+xx*(c2+xx*(c3+xx*(c4+xx*(c5+xx*(c6+xx*(c7+xx*(c8+xx*c9))))))));
    return((xx-1.0)*(xx-4.0)*(xx-9.0)*(xx-16.0)*p);
#else
    /*
      Max. abs. rel. error 1.2e-12 < 1/2^39.
    */
    const double c0 = 0.173611111110910715186413700076827593074e-2L;
    const double c1 = -0.289105544717893415815859968653611245425e-3L;
    const double c2 = 0.206952161241815727624413291940849294025e-4L;
    const double c3 = -0.834446180169727178193268528095341741698e-6L;
    const double c4 = 0.207010104171026718629622453275917944941e-7L;
    const double c5 = -0.319724784938507108101517564300855542655e-9L;
    const double c6 = 0.288101675249103266147006509214934493930e-11L;
    const double c7 = -0.118218971804934245819960233886876537953e-13L;
    const double p =
      c0+xx*(c1+xx*(c2+xx*(c3+xx*(c4+xx*(c5+xx*(c6+xx*c7))))));
    const double d0 = 1.0L;
    const double d1 = 0.547981619622284827495856984100563583948e-1L;
    const double d2 = 0.134226268835357312626304688047086921806e-2L;
    const double d3 = 0.178994697503371051002463656833597608689e-4L;
    const double d4 = 0.114633394140438168641246022557689759090e-6L;
    const double q = d0+xx*(d1+xx*(d2+xx*(d3+xx*d4)));
    return((xx-1.0)*(xx-4.0)*(xx-9.0)*(xx-16.0)/q*p);
#endif
  }
}