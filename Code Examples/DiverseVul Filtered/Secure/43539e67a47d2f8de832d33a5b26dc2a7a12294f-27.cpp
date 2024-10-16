static inline void Hq2X(const Image *source,const Quantum *pixels,
  Quantum *result,const size_t channels)
{
  static const unsigned int
    Hq2XTable[] =
    {
      4, 4, 6,  2, 4, 4, 6,  2, 5,  3, 15, 12, 5,  3, 17, 13,
      4, 4, 6, 18, 4, 4, 6, 18, 5,  3, 12, 12, 5,  3,  1, 12,
      4, 4, 6,  2, 4, 4, 6,  2, 5,  3, 17, 13, 5,  3, 16, 14,
      4, 4, 6, 18, 4, 4, 6, 18, 5,  3, 16, 12, 5,  3,  1, 14,
      4, 4, 6,  2, 4, 4, 6,  2, 5, 19, 12, 12, 5, 19, 16, 12,
      4, 4, 6,  2, 4, 4, 6,  2, 5,  3, 16, 12, 5,  3, 16, 12,
      4, 4, 6,  2, 4, 4, 6,  2, 5, 19,  1, 12, 5, 19,  1, 14,
      4, 4, 6,  2, 4, 4, 6, 18, 5,  3, 16, 12, 5, 19,  1, 14,
      4, 4, 6,  2, 4, 4, 6,  2, 5,  3, 15, 12, 5,  3, 17, 13,
      4, 4, 6,  2, 4, 4, 6,  2, 5,  3, 16, 12, 5,  3, 16, 12,
      4, 4, 6,  2, 4, 4, 6,  2, 5,  3, 17, 13, 5,  3, 16, 14,
      4, 4, 6,  2, 4, 4, 6,  2, 5,  3, 16, 13, 5,  3,  1, 14,
      4, 4, 6,  2, 4, 4, 6,  2, 5,  3, 16, 12, 5,  3, 16, 13,
      4, 4, 6,  2, 4, 4, 6,  2, 5,  3, 16, 12, 5,  3,  1, 12,
      4, 4, 6,  2, 4, 4, 6,  2, 5,  3, 16, 12, 5,  3,  1, 14,
      4, 4, 6,  2, 4, 4, 6,  2, 5,  3,  1, 12, 5,  3,  1, 14
    };

  const int
    pattern1[] =
    {
      !PixelsEqual(pixels,4,pixels,8,channels),
      !PixelsEqual(pixels,4,pixels,7,channels),
      !PixelsEqual(pixels,4,pixels,6,channels),
      !PixelsEqual(pixels,4,pixels,5,channels),
      !PixelsEqual(pixels,4,pixels,3,channels),
      !PixelsEqual(pixels,4,pixels,2,channels),
      !PixelsEqual(pixels,4,pixels,1,channels),
      !PixelsEqual(pixels,4,pixels,0,channels)
    };

#define Rotated(p) p[2], p[4], p[7], p[1], p[6], p[0], p[3], p[5]
  const int pattern2[] = { Rotated(pattern1) };
  const int pattern3[] = { Rotated(pattern2) };
  const int pattern4[] = { Rotated(pattern3) };
#undef Rotated

  Hq2XHelper(Hq2XTable[Hq2XPatternToNumber(pattern1)],pixels,result,0,
    channels,4,0,1,3,5,7);
  Hq2XHelper(Hq2XTable[Hq2XPatternToNumber(pattern2)],pixels,result,1,
    channels,4,2,5,1,7,3);
  Hq2XHelper(Hq2XTable[Hq2XPatternToNumber(pattern3)],pixels,result,3,
    channels,4,8,7,5,3,1);
  Hq2XHelper(Hq2XTable[Hq2XPatternToNumber(pattern4)],pixels,result,2,
    channels,4,6,3,7,1,5);
}