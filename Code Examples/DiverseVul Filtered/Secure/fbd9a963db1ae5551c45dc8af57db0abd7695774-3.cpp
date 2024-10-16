static inline void ConvertLCHuvToXYZ(const double luma,const double chroma,
  const double hue,double *X,double *Y,double *Z)
{
  ConvertLuvToXYZ(luma,chroma*cos(hue*MagickPI/180.0),chroma*
    sin(hue*MagickPI/180.0),X,Y,Z);
}