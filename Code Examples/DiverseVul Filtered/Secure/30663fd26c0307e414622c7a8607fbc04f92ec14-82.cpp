static inline void gen_ldq_env_A0(DisasContext *s, int offset)
{
    tcg_gen_qemu_ld_i64(cpu_tmp1_i64, cpu_A0, s->mem_index, MO_LEQ);
    tcg_gen_st_i64(cpu_tmp1_i64, cpu_env, offset);
}