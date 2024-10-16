static void gen_inc(DisasContext *s1, TCGMemOp ot, int d, int c)
{
    if (s1->prefix & PREFIX_LOCK) {
        tcg_gen_movi_tl(cpu_T0, c > 0 ? 1 : -1);
        tcg_gen_atomic_add_fetch_tl(cpu_T0, cpu_A0, cpu_T0,
                                    s1->mem_index, ot | MO_LE);
    } else {
        if (d != OR_TMP0) {
            gen_op_mov_v_reg(ot, cpu_T0, d);
        } else {
            gen_op_ld_v(s1, ot, cpu_T0, cpu_A0);
        }
        tcg_gen_addi_tl(cpu_T0, cpu_T0, (c > 0 ? 1 : -1));
        gen_op_st_rm_T0_A0(s1, ot, d);
    }

    gen_compute_eflags_c(s1, cpu_cc_src);
    tcg_gen_mov_tl(cpu_cc_dst, cpu_T0);
    set_cc_op(s1, (c > 0 ? CC_OP_INCB : CC_OP_DECB) + ot);
}