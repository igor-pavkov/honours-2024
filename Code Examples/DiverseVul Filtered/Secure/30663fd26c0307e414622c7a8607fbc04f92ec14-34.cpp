static inline void gen_op_st_rm_T0_A0(DisasContext *s, int idx, int d)
{
    if (d == OR_TMP0) {
        gen_op_st_v(s, idx, cpu_T0, cpu_A0);
    } else {
        gen_op_mov_reg_v(idx, d, cpu_T0);
    }
}