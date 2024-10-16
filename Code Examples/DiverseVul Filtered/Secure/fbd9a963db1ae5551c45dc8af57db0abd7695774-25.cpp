static inline void ConvertXYZToLCHab(const double X,const double Y,
  const double Z,double *luma,double *chroma,double *hue)
{
  double
    a,
    b;

  ConvertXYZToLab(X,Y,Z,luma,&a,&b);
  *chroma=hypot(255.0*(a-0.5),255.0*(b-0.5))/255.0+0.5;
  *hue=180.0*atan2(255.0*(b-0.5),255.0*(a-0.5))/MagickPI/360.0;
  if (*hue < 0.0)
    *hue+=1.0;
}