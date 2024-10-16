static inline void gen_op_mov_v_reg(TCGMemOp ot, TCGv t0, int reg)
{
    if (ot == MO_8 && byte_reg_is_xH(reg)) {
        tcg_gen_extract_tl(t0, cpu_regs[reg - 4], 8, 8);
    } else {
        tcg_gen_mov_tl(t0, cpu_regs[reg]);
    }
}