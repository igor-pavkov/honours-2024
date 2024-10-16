static inline void Scale3X(const Image *source,const Quantum *pixels,
  Quantum *result,const size_t channels)
{
  if (!PixelsEqual(pixels,1,pixels,7,channels) &&
      !PixelsEqual(pixels,3,pixels,5,channels))
    {
      if (PixelsEqual(pixels,3,pixels,1,channels))
        CopyPixels(pixels,3,result,0,channels);
      else
        CopyPixels(pixels,4,result,0,channels);

      if (
        (
          PixelsEqual(pixels,3,pixels,1,channels) &&
          !PixelsEqual(pixels,4,pixels,2,channels)
        ) ||
        (
          PixelsEqual(pixels,5,pixels,1,channels) &&
          !PixelsEqual(pixels,4,pixels,0,channels)
        )
      )
        CopyPixels(pixels,1,result,1,channels);
      else
        CopyPixels(pixels,4,result,1,channels);
      if (PixelsEqual(pixels,5,pixels,1,channels))
        CopyPixels(pixels,5,result,2,channels);
      else
        CopyPixels(pixels,4,result,2,channels);
      if (
        (
          PixelsEqual(pixels,3,pixels,1,channels) &&
          !PixelsEqual(pixels,4,pixels,6,channels)
        ) ||
        (
          PixelsEqual(pixels,3,pixels,7,channels) &&
          !PixelsEqual(pixels,4,pixels,0,channels)
        )
      )
        CopyPixels(pixels,3,result,3,channels);
      else
        CopyPixels(pixels,4,result,3,channels);
      CopyPixels(pixels,4,result,4,channels);
      if (
        (
          PixelsEqual(pixels,5,pixels,1,channels) &&
          !PixelsEqual(pixels,4,pixels,8,channels)
        ) ||
        (
          PixelsEqual(pixels,5,pixels,7,channels) &&
          !PixelsEqual(pixels,4,pixels,2,channels)
        )
      )
        CopyPixels(pixels,5,result,5,channels);
      else
        CopyPixels(pixels,4,result,5,channels);
      if (PixelsEqual(pixels,3,pixels,7,channels))
        CopyPixels(pixels,3,result,6,channels);
      else
        CopyPixels(pixels,4,result,6,channels);
      if (
        (
          PixelsEqual(pixels,3,pixels,7,channels) &&
          !PixelsEqual(pixels,4,pixels,8,channels)
        ) ||
        (
          PixelsEqual(pixels,5,pixels,7,channels) &&
          !PixelsEqual(pixels,4,pixels,6,channels)
        )
      )
        CopyPixels(pixels,7,result,7,channels);
      else
        CopyPixels(pixels,4,result,7,channels);
      if (PixelsEqual(pixels,5,pixels,7,channels))
        CopyPixels(pixels,5,result,8,channels);
      else
        CopyPixels(pixels,4,result,8,channels);
    }
  else
    {
      register ssize_t
        i;

      for (i=0; i < 9; i++)
        CopyPixels(pixels,4,result,i,channels);
    }
}