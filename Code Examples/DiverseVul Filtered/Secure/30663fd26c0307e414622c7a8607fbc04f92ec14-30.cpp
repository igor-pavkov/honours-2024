static inline void gen_op_movl_T0_Dshift(TCGMemOp ot)
{
    tcg_gen_ld32s_tl(cpu_T0, cpu_env, offsetof(CPUX86State, df));
    tcg_gen_shli_tl(cpu_T0, cpu_T0, ot);
};