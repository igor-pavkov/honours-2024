static inline void gen_op_testl_T0_T1_cc(void)
{
    tcg_gen_and_tl(cpu_cc_dst, cpu_T0, cpu_T1);
}