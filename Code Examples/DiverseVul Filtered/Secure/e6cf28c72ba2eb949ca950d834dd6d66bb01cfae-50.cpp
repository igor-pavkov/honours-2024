ALWAYS_INLINE void ScalarMulAdd(const float a, const bfloat16** inp,
                                float** out) {
  float inp_f = ConvertBfloat16ToFloat(*inp);
  **out += a * inp_f;
  ++*inp;
  ++*out;
}