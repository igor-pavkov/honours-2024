static inline void ConvertXYZToLCHuv(const double X,const double Y,
  const double Z,double *luma,double *chroma,double *hue)
{
  double
    u,
    v;

  ConvertXYZToLuv(X,Y,Z,luma,&u,&v);
  *chroma=hypot(354.0*u-134.0,262.0*v-140.0)/255.0+0.5;
  *hue=180.0*atan2(262.0*v-140.0,354.0*u-134.0)/MagickPI/360.0;
  if (*hue < 0.0)
    *hue+=1.0;
}