static void gen_op_update2_cc(void)
{
    tcg_gen_mov_tl(cpu_cc_src, cpu_T1);
    tcg_gen_mov_tl(cpu_cc_dst, cpu_T0);
}