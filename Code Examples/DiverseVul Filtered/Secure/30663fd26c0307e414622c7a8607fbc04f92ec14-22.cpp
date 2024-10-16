static inline void gen_stos(DisasContext *s, TCGMemOp ot)
{
    gen_op_mov_v_reg(MO_32, cpu_T0, R_EAX);
    gen_string_movl_A0_EDI(s);
    gen_op_st_v(s, ot, cpu_T0, cpu_A0);
    gen_op_movl_T0_Dshift(ot);
    gen_op_add_reg_T0(s->aflag, R_EDI);
}