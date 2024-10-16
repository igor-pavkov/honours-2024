static void Epbx2X(const Image *source,const Quantum *pixels,
  Quantum *result,const size_t channels)
{
#define HelperCond(a,b,c,d,e,f,g) ( \
  PixelsEqual(pixels,a,pixels,b,channels) && ( \
    PixelsEqual(pixels,c,pixels,d,channels) || \
    PixelsEqual(pixels,c,pixels,e,channels) || \
    PixelsEqual(pixels,a,pixels,f,channels) || \
    PixelsEqual(pixels,b,pixels,g,channels) \
    ) \
  )

  register ssize_t
    i;

  for (i=0; i < 4; i++)
    CopyPixels(pixels,4,result,i,channels);
  if (
    !PixelsEqual(pixels,3,pixels,5,channels) &&
    !PixelsEqual(pixels,1,pixels,7,channels) &&
    (
      PixelsEqual(pixels,4,pixels,3,channels) ||
      PixelsEqual(pixels,4,pixels,7,channels) ||
      PixelsEqual(pixels,4,pixels,5,channels) ||
      PixelsEqual(pixels,4,pixels,1,channels) ||
      (
        (
          !PixelsEqual(pixels,0,pixels,8,channels) ||
           PixelsEqual(pixels,4,pixels,6,channels) ||
           PixelsEqual(pixels,3,pixels,2,channels)
        ) &&
        (
          !PixelsEqual(pixels,6,pixels,2,channels) ||
           PixelsEqual(pixels,4,pixels,0,channels) ||
           PixelsEqual(pixels,4,pixels,8,channels)
        )
      )
    )
  )
    {
      if (HelperCond(1,3,4,0,8,2,6))
        Mix2Pixels(pixels,1,3,result,0,channels);
      if (HelperCond(5,1,4,2,6,8,0))
        Mix2Pixels(pixels,5,1,result,1,channels);
      if (HelperCond(3,7,4,6,2,0,8))
        Mix2Pixels(pixels,3,7,result,2,channels);
      if (HelperCond(7,5,4,8,0,6,2))
        Mix2Pixels(pixels,7,5,result,3,channels);
    }

#undef HelperCond
}