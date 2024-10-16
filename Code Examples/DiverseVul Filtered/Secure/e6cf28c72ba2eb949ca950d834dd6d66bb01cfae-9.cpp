ALWAYS_INLINE void ScalarMulAdd3Way(const float a1, const float a2,
                                    const float a3, const float** inp1,
                                    const float** inp2, const float** inp3,
                                    float** out) {
  **out += a1 * **inp1 + a2 * **inp2 + a3 * **inp3;
  ++*out;
  ++*inp1;
  ++*inp2;
  ++*inp3;
}