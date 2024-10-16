ALWAYS_INLINE void ScalarMulAdd(const float a, const float** inp, float** out) {
  **out += a * **inp;
  ++*inp;
  ++*out;
}