static inline void gen_op_add_reg_T0(TCGMemOp size, int reg)
{
    tcg_gen_add_tl(cpu_tmp0, cpu_regs[reg], cpu_T0);
    gen_op_mov_reg_v(size, reg, cpu_tmp0);
}