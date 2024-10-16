static inline void gen_scas(DisasContext *s, TCGMemOp ot)
{
    gen_string_movl_A0_EDI(s);
    gen_op_ld_v(s, ot, cpu_T1, cpu_A0);
    gen_op(s, OP_CMPL, ot, R_EAX);
    gen_op_movl_T0_Dshift(ot);
    gen_op_add_reg_T0(s->aflag, R_EDI);
}