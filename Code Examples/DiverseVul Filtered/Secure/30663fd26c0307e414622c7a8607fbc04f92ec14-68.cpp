static TCGMemOp gen_pop_T0(DisasContext *s)
{
    TCGMemOp d_ot = mo_pushpop(s, s->dflag);

    gen_lea_v_seg(s, mo_stacksize(s), cpu_regs[R_ESP], R_SS, -1);
    gen_op_ld_v(s, d_ot, cpu_T0, cpu_A0);

    return d_ot;
}