static inline unsigned char *PopQuantumPixel(QuantumInfo *quantum_info,
  const QuantumAny pixel,unsigned char *magick_restrict pixels)
{
  ssize_t
    i;

  size_t
    quantum_bits;

  if (quantum_info->state.bits == 0UL)
    quantum_info->state.bits=8U;
  for (i=(ssize_t) quantum_info->depth; i > 0L; )
  {
    quantum_bits=(size_t) i;
    if (quantum_bits > quantum_info->state.bits)
      quantum_bits=quantum_info->state.bits;
    i-=(ssize_t) quantum_bits;
    if (i < 0)
      i=0;
    if (quantum_info->state.bits == 8UL)
      *pixels='\0';
    quantum_info->state.bits-=quantum_bits;
    *pixels|=(((pixel >> i) &~ ((~0UL) << quantum_bits)) <<
      quantum_info->state.bits);
    if (quantum_info->state.bits == 0UL)
      {
        pixels++;
        quantum_info->state.bits=8UL;
      }
  }
  return(pixels);
}