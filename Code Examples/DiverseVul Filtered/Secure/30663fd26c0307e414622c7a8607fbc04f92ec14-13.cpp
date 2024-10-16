static inline void gen_stq_env_A0(DisasContext *s, int offset)
{
    tcg_gen_ld_i64(cpu_tmp1_i64, cpu_env, offset);
    tcg_gen_qemu_st_i64(cpu_tmp1_i64, cpu_A0, s->mem_index, MO_LEQ);
}