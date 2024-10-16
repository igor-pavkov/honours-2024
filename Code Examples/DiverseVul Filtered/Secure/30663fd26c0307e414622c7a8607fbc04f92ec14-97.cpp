static void gen_op_update1_cc(void)
{
    tcg_gen_mov_tl(cpu_cc_dst, cpu_T0);
}