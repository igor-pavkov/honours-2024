static inline void Eagle2X(const Image *source,const Quantum *pixels,
  Quantum *result,const size_t channels)
{
  ssize_t
    i;

  (void) source;
  for (i=0; i < 4; i++)
    CopyPixels(pixels,4,result,i,channels);
  if (PixelsEqual(pixels,0,pixels,1,channels) &&
      PixelsEqual(pixels,1,pixels,3,channels))
    CopyPixels(pixels,0,result,0,channels);
  if (PixelsEqual(pixels,1,pixels,2,channels) &&
      PixelsEqual(pixels,2,pixels,5,channels))
    CopyPixels(pixels,2,result,1,channels);
  if (PixelsEqual(pixels,3,pixels,6,channels) &&
      PixelsEqual(pixels,6,pixels,7,channels))
    CopyPixels(pixels,6,result,2,channels);
  if (PixelsEqual(pixels,5,pixels,8,channels) &&
      PixelsEqual(pixels,8,pixels,7,channels))
    CopyPixels(pixels,8,result,3,channels);
}