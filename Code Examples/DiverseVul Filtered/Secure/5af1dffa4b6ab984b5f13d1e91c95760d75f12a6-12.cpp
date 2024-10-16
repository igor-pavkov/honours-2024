static inline unsigned char *PopFloatPixel(QuantumInfo *quantum_info,
  const float pixel,unsigned char *magick_restrict pixels)
{
  float
    *p;

  unsigned char
    quantum[4];

  (void) memset(quantum,0,sizeof(quantum));
  p=(float *) quantum;
  *p=(float) ((double) pixel*quantum_info->state.inverse_scale+
    quantum_info->minimum);
  if (quantum_info->endian == LSBEndian)
    {
      *pixels++=quantum[0];
      *pixels++=quantum[1];
      *pixels++=quantum[2];
      *pixels++=quantum[3];
      return(pixels);
    }
  *pixels++=quantum[3];
  *pixels++=quantum[2];
  *pixels++=quantum[1];
  *pixels++=quantum[0];
  return(pixels);
}