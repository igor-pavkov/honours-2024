inline uint8x8_t ComputeLerpx8Tmpl(const quint8* const yl, const quint8* yu,
                                   const int64* xl, const int64* xu,
                                   const int16* xlp,
                                   const int16x8_t ys_lerpsx) {
  return ComputeLerpx8<RESOLUTION>(
      yl + xl[ID0] + CH0, yl + xu[ID0] + CH0, yu + xl[ID0] + CH0,
      yu + xu[ID0] + CH0, xlp + ID0, yl + xl[ID1] + CH1, yl + xu[ID1] + CH1,
      yu + xl[ID1] + CH1, yu + xu[ID1] + CH1, xlp + ID1, yl + xl[ID2] + CH2,
      yl + xu[ID2] + CH2, yu + xl[ID2] + CH2, yu + xu[ID2] + CH2, xlp + ID2,
      yl + xl[ID3] + CH3, yl + xu[ID3] + CH3, yu + xl[ID3] + CH3,
      yu + xu[ID3] + CH3, xlp + ID3, yl + xl[ID4] + CH4, yl + xu[ID4] + CH4,
      yu + xl[ID4] + CH4, yu + xu[ID4] + CH4, xlp + ID4, yl + xl[ID5] + CH5,
      yl + xu[ID5] + CH5, yu + xl[ID5] + CH5, yu + xu[ID5] + CH5, xlp + ID5,
      yl + xl[ID6] + CH6, yl + xu[ID6] + CH6, yu + xl[ID6] + CH6,
      yu + xu[ID6] + CH6, xlp + ID6, yl + xl[ID7] + CH7, yl + xu[ID7] + CH7,
      yu + xl[ID7] + CH7, yu + xu[ID7] + CH7, xlp + ID7, ys_lerpsx);
}