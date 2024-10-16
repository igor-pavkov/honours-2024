static void gen_op_update3_cc(TCGv reg)
{
    tcg_gen_mov_tl(cpu_cc_src2, reg);
    tcg_gen_mov_tl(cpu_cc_src, cpu_T1);
    tcg_gen_mov_tl(cpu_cc_dst, cpu_T0);
}