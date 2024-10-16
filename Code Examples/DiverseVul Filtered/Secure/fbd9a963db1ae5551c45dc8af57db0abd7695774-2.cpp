MagickPrivate void ConvertRGBToLab(const double red,const double green,
  const double blue,double *L,double *a,double *b)
{
  double
    X,
    Y,
    Z;

  ConvertRGBToXYZ(red,green,blue,&X,&Y,&Z);
  ConvertXYZToLab(X,Y,Z,L,a,b);
}