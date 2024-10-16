inline uint8x8_t ComputeLerpx8(
    const quint8* tl0, const quint8* tr0, const quint8* bl0, const quint8* br0,
    const int16* xlp0, const quint8* tl1, const quint8* tr1, const quint8* bl1,
    const quint8* br1, const int16* xlp1, const quint8* tl2, const quint8* tr2,
    const quint8* bl2, const quint8* br2, const int16* xlp2, const quint8* tl3,
    const quint8* tr3, const quint8* bl3, const quint8* br3, const int16* xlp3,
    const quint8* tl4, const quint8* tr4, const quint8* bl4, const quint8* br4,
    const int16* xlp4, const quint8* tl5, const quint8* tr5, const quint8* bl5,
    const quint8* br5, const int16* xlp5, const quint8* tl6, const quint8* tr6,
    const quint8* bl6, const quint8* br6, const int16* xlp6, const quint8* tl7,
    const quint8* tr7, const quint8* bl7, const quint8* br7, const int16* xlp7,
    const int16x8_t ys_lerpsx) {
  const uint8x8_t tl8x8 = ToUint8x8(tl0, tl1, tl2, tl3, tl4, tl5, tl6, tl7);
  const uint8x8_t tr8x8 = ToUint8x8(tr0, tr1, tr2, tr3, tr4, tr5, tr6, tr7);
  const uint8x8_t bl8x8 = ToUint8x8(bl0, bl1, bl2, bl3, bl4, bl5, bl6, bl7);
  const uint8x8_t br8x8 = ToUint8x8(br0, br1, br2, br3, br4, br5, br6, br7);
  const int16x8_t xs_lerpsx =
      ToInt16x8(xlp0, xlp1, xlp2, xlp3, xlp4, xlp5, xlp6, xlp7);
  return ComputeLerp8x8<RESOLUTION>(tl8x8, tr8x8, bl8x8, br8x8, xs_lerpsx,
                                    ys_lerpsx);
}