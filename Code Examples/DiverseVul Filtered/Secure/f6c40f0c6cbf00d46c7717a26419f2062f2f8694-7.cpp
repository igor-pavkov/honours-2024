inline T_CALC MulOffset(T a, T b, T_SCALE c) {
  return (static_cast<T_CALC>(a) - static_cast<T_CALC>(b)) *
         static_cast<T_CALC>(c);
}