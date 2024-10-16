static inline void gen_op_movl_seg_T0_vm(int seg_reg)
{
    tcg_gen_ext16u_tl(cpu_T0, cpu_T0);
    tcg_gen_st32_tl(cpu_T0, cpu_env,
                    offsetof(CPUX86State,segs[seg_reg].selector));
    tcg_gen_shli_tl(cpu_seg_base[seg_reg], cpu_T0, 4);
}