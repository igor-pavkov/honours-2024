static void Xbr2X(const Image *source,const Quantum *pixels,Quantum *result,
  const size_t channels)
{
#define WeightVar(M,N) const int w_##M##_##N = \
  PixelsEqual(pixels,M,pixels,N,channels) ? 0 : 1;

  WeightVar(12,11)
  WeightVar(12,7)
  WeightVar(12,13)
  WeightVar(12,17)
  WeightVar(12,16)
  WeightVar(12,8)
  WeightVar(6,10)
  WeightVar(6,2)
  WeightVar(11,7)
  WeightVar(11,17)
  WeightVar(11,5)
  WeightVar(7,13)
  WeightVar(7,1)
  WeightVar(12,6)
  WeightVar(12,18)
  WeightVar(8,14)
  WeightVar(8,2)
  WeightVar(13,17)
  WeightVar(13,9)
  WeightVar(7,3)
  WeightVar(16,10)
  WeightVar(16,22)
  WeightVar(17,21)
  WeightVar(11,15)
  WeightVar(18,14)
  WeightVar(18,22)
  WeightVar(17,23)
  WeightVar(17,19)
#undef WeightVar

  if (
    w_12_16 + w_12_8 + w_6_10 + w_6_2 + (4 * w_11_7) <
    w_11_17 + w_11_5 + w_7_13 + w_7_1 + (4 * w_12_6)
  )
    Mix2Pixels(pixels,(ssize_t) (w_12_11 <= w_12_7 ? 11 : 7),12,result,0,
      channels);
  else
    CopyPixels(pixels,12,result,0,channels);
  if (
    w_12_18 + w_12_6 + w_8_14 + w_8_2 + (4 * w_7_13) <
    w_13_17 + w_13_9 + w_11_7 + w_7_3 + (4 * w_12_8)
  )
    Mix2Pixels(pixels,(ssize_t) (w_12_7 <= w_12_13 ? 7 : 13),12,result,1,
      channels);
  else
    CopyPixels(pixels,12,result,1,channels);
  if (
    w_12_6 + w_12_18 + w_16_10 + w_16_22 + (4 * w_11_17) <
    w_11_7 + w_11_15 + w_13_17 + w_17_21 + (4 * w_12_16)
  )
    Mix2Pixels(pixels,(ssize_t) (w_12_11 <= w_12_17 ? 11 : 17),12,result,2,
      channels);
  else
    CopyPixels(pixels,12,result,2,channels);
  if (
    w_12_8 + w_12_16 + w_18_14 + w_18_22 + (4 * w_13_17) <
    w_11_17 + w_17_23 + w_17_19 + w_7_13 + (4 * w_12_18)
  )
    Mix2Pixels(pixels,(ssize_t) (w_12_13 <= w_12_17 ? 13 : 17),12,result,3,
      channels);
  else
    CopyPixels(pixels,12,result,3,channels);
}