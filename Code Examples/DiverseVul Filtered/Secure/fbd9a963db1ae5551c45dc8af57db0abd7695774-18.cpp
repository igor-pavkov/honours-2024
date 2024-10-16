static inline void ConvertLCHabToXYZ(const double luma,const double chroma,
  const double hue,double *X,double *Y,double *Z)
{
  ConvertLabToXYZ(luma,chroma*cos(hue*MagickPI/180.0),chroma*
    sin(hue*MagickPI/180.0),X,Y,Z);
}