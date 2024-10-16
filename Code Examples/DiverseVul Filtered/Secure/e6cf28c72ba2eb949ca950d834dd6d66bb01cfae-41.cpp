ALWAYS_INLINE void ScalarMulAdd3Way(const float a1, const float a2,
                                    const float a3, const bfloat16** inp1,
                                    const bfloat16** inp2,
                                    const bfloat16** inp3, float** out) {
  float inp1_f = ConvertBfloat16ToFloat(*inp1);
  float inp2_f = ConvertBfloat16ToFloat(*inp2);
  float inp3_f = ConvertBfloat16ToFloat(*inp3);
  **out += a1 * inp1_f + a2 * inp2_f + a3 * inp3_f;
  ++*out;
  ++*inp1;
  ++*inp2;
  ++*inp3;
}