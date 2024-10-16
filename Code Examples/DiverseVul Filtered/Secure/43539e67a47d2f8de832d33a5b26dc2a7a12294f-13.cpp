static inline void Eagle3X(const Image *source,const Quantum *pixels,
  Quantum *result,const size_t channels)
{
  ssize_t
    corner_tl,
    corner_tr,
    corner_bl,
    corner_br;

  corner_tl=PixelsEqual(pixels,0,pixels,1,channels) &&
    PixelsEqual(pixels,0,pixels,3,channels);
  corner_tr=PixelsEqual(pixels,1,pixels,2,channels) &&
    PixelsEqual(pixels,2,pixels,5,channels);
  corner_bl=PixelsEqual(pixels,3,pixels,6,channels) &&
    PixelsEqual(pixels,6,pixels,7,channels);
  corner_br=PixelsEqual(pixels,5,pixels,7,channels) &&
    PixelsEqual(pixels,7,pixels,8,channels);
  CopyPixels(pixels,(ssize_t) (corner_tl ? 0 : 4),result,0,channels);
  if (corner_tl && corner_tr)
    Mix2Pixels(pixels,0,2,result,1,channels);
  else
    CopyPixels(pixels,4,result,1,channels);
  CopyPixels(pixels,(ssize_t) (corner_tr ? 1 : 4),result,2,channels);
  if (corner_tl && corner_bl)
    Mix2Pixels(pixels,0,6,result,3,channels);
  else
    CopyPixels(pixels,4,result,3,channels);
  CopyPixels(pixels,4,result,4,channels);
  if (corner_tr && corner_br)
    Mix2Pixels(pixels,2,8,result,5,channels);
  else
    CopyPixels(pixels,4,result,5,channels);
  CopyPixels(pixels,(ssize_t) (corner_bl ? 3 : 4),result,6,channels);
  if (corner_bl && corner_br)
    Mix2Pixels(pixels,6,8,result,7,channels);
  else
    CopyPixels(pixels,4,result,7,channels);
  CopyPixels(pixels,(ssize_t) (corner_br ? 5 : 4),result,8,channels);
}