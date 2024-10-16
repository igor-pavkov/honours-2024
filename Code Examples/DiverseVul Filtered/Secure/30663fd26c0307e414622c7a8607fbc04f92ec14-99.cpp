static void gen_leave(DisasContext *s)
{
    TCGMemOp d_ot = mo_pushpop(s, s->dflag);
    TCGMemOp a_ot = mo_stacksize(s);

    gen_lea_v_seg(s, a_ot, cpu_regs[R_EBP], R_SS, -1);
    gen_op_ld_v(s, d_ot, cpu_T0, cpu_A0);

    tcg_gen_addi_tl(cpu_T1, cpu_regs[R_EBP], 1 << d_ot);

    gen_op_mov_reg_v(d_ot, R_EBP, cpu_T0);
    gen_op_mov_reg_v(a_ot, R_ESP, cpu_T1);
}