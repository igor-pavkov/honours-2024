static inline void gen_jmp_im(target_ulong pc)
{
    tcg_gen_movi_tl(cpu_tmp0, pc);
    gen_op_jmp_v(cpu_tmp0);
}