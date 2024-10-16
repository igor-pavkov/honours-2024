static inline void gen_lods(DisasContext *s, TCGMemOp ot)
{
    gen_string_movl_A0_ESI(s);
    gen_op_ld_v(s, ot, cpu_T0, cpu_A0);
    gen_op_mov_reg_v(ot, R_EAX, cpu_T0);
    gen_op_movl_T0_Dshift(ot);
    gen_op_add_reg_T0(s->aflag, R_ESI);
}