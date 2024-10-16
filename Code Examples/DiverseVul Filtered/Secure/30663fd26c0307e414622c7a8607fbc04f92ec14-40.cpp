static inline void gen_op_movl(int d_offset, int s_offset)
{
    tcg_gen_ld_i32(cpu_tmp2_i32, cpu_env, s_offset);
    tcg_gen_st_i32(cpu_tmp2_i32, cpu_env, d_offset);
}