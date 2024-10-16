static inline void gen_op_movo(int d_offset, int s_offset)
{
    tcg_gen_ld_i64(cpu_tmp1_i64, cpu_env, s_offset + offsetof(ZMMReg, ZMM_Q(0)));
    tcg_gen_st_i64(cpu_tmp1_i64, cpu_env, d_offset + offsetof(ZMMReg, ZMM_Q(0)));
    tcg_gen_ld_i64(cpu_tmp1_i64, cpu_env, s_offset + offsetof(ZMMReg, ZMM_Q(1)));
    tcg_gen_st_i64(cpu_tmp1_i64, cpu_env, d_offset + offsetof(ZMMReg, ZMM_Q(1)));
}