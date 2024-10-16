static inline void gen_movs(DisasContext *s, TCGMemOp ot)
{
    gen_string_movl_A0_ESI(s);
    gen_op_ld_v(s, ot, cpu_T0, cpu_A0);
    gen_string_movl_A0_EDI(s);
    gen_op_st_v(s, ot, cpu_T0, cpu_A0);
    gen_op_movl_T0_Dshift(ot);
    gen_op_add_reg_T0(s->aflag, R_ESI);
    gen_op_add_reg_T0(s->aflag, R_EDI);
}