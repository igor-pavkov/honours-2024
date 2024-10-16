static inline void gen_op_movl_T0_seg(int seg_reg)
{
    tcg_gen_ld32u_tl(cpu_T0, cpu_env,
                     offsetof(CPUX86State,segs[seg_reg].selector));
}