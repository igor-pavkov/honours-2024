static inline void gen_op_add_reg_im(TCGMemOp size, int reg, int32_t val)
{
    tcg_gen_addi_tl(cpu_tmp0, cpu_regs[reg], val);
    gen_op_mov_reg_v(size, reg, cpu_tmp0);
}