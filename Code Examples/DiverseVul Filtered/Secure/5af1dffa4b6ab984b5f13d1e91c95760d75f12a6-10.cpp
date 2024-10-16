static inline unsigned char *PopDoublePixel(QuantumInfo *quantum_info,
  const double pixel,unsigned char *magick_restrict pixels)
{
  double
    *p;

  unsigned char
    quantum[8];

  (void) memset(quantum,0,sizeof(quantum));
  p=(double *) quantum;
  *p=(double) (pixel*quantum_info->state.inverse_scale+quantum_info->minimum);
  if (quantum_info->endian == LSBEndian)
    {
      *pixels++=quantum[0];
      *pixels++=quantum[1];
      *pixels++=quantum[2];
      *pixels++=quantum[3];
      *pixels++=quantum[4];
      *pixels++=quantum[5];
      *pixels++=quantum[6];
      *pixels++=quantum[7];
      return(pixels);
    }
  *pixels++=quantum[7];
  *pixels++=quantum[6];
  *pixels++=quantum[5];
  *pixels++=quantum[4];
  *pixels++=quantum[3];
  *pixels++=quantum[2];
  *pixels++=quantum[1];
  *pixels++=quantum[0];
  return(pixels);
}