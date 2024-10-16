static void Fish2X(const Image *source,const Quantum *pixels,Quantum *result,
  const size_t channels)
{
#define Corner(A,B,C,D) \
  { \
    if (intensities[B] > intensities[A]) \
      { \
        ssize_t    \
          offsets[3] = { B, C, D }; \
 \
        MixPixels(pixels,offsets,3,result,3,channels); \
      } \
    else \
      { \
        ssize_t    \
          offsets[3] = { A, B, C }; \
 \
        MixPixels(pixels,offsets,3,result,3,channels); \
      } \
  }

#define Line(A,B,C,D) \
  { \
    if (intensities[C] > intensities[A]) \
      Mix2Pixels(pixels,C,D,result,3,channels); \
    else \
      Mix2Pixels(pixels,A,B,result,3,channels); \
  }

  MagickFloatType
    intensities[9];

  int
    ae,
    bd,
    ab,
    ad,
    be,
    de;

  register ssize_t
    i;

  ssize_t
    offsets[4] = { 0, 1, 3, 4 };

  for (i=0; i < 9; i++)
    intensities[i]=GetPixelIntensity(source,pixels + i*channels);
  CopyPixels(pixels,0,result,0,channels);
  CopyPixels(pixels,(ssize_t) (intensities[0] > intensities[1] ? 0 : 1),result,
    1,channels);
  CopyPixels(pixels,(ssize_t) (intensities[0] > intensities[3] ? 0 : 3),result,
    2,channels);
  ae=PixelsEqual(pixels,0,pixels,4,channels);
  bd=PixelsEqual(pixels,1,pixels,3,channels);
  ab=PixelsEqual(pixels,0,pixels,1,channels);
  de=PixelsEqual(pixels,3,pixels,4,channels);
  ad=PixelsEqual(pixels,0,pixels,3,channels);
  be=PixelsEqual(pixels,1,pixels,4,channels);
  if (ae && bd && ab)
    {
      CopyPixels(pixels,0,result,3,channels);
      return;
    }
  if (ad && de && !ab)
    {
      Corner(1,0,4,3)
      return;
    }
  if (be && de && !ab)
    {
      Corner(0,1,3,4)
      return;
    }
  if (ad && ab && !be)
    {
      Corner(4,3,1,0)
      return;
    }
  if (ab && be && !ad)
    {
      Corner(3,0,4,1)
      return;
    }
  if (ae && (!bd || intensities[1] > intensities[0]))
    {
      Mix2Pixels(pixels,0,4,result,3,channels);
      return;
    }
  if (bd && (!ae || intensities[0] > intensities[1]))
    {
      Mix2Pixels(pixels,1,3,result,3,channels);
      return;
    }
  if (ab)
    {
      Line(0,1,3,4)
      return;
    }
  if (de)
    {
      Line(3,4,0,1)
      return;
    }
  if (ad)
    {
      Line(0,3,1,4)
      return;
    }
  if (be)
    {
      Line(1,4,0,3)
      return;
    }
  MixPixels(pixels,offsets,4,result,3,channels);
#undef Corner
#undef Line
}