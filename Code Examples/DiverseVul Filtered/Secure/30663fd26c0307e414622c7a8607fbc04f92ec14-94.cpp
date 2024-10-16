static inline void gen_ldo_env_A0(DisasContext *s, int offset)
{
    int mem_index = s->mem_index;
    tcg_gen_qemu_ld_i64(cpu_tmp1_i64, cpu_A0, mem_index, MO_LEQ);
    tcg_gen_st_i64(cpu_tmp1_i64, cpu_env, offset + offsetof(ZMMReg, ZMM_Q(0)));
    tcg_gen_addi_tl(cpu_tmp0, cpu_A0, 8);
    tcg_gen_qemu_ld_i64(cpu_tmp1_i64, cpu_tmp0, mem_index, MO_LEQ);
    tcg_gen_st_i64(cpu_tmp1_i64, cpu_env, offset + offsetof(ZMMReg, ZMM_Q(1)));
}