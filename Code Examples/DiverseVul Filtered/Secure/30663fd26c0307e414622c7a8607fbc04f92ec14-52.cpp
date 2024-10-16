static void gen_shift_rm_im(DisasContext *s, TCGMemOp ot, int op1, int op2,
                            int is_right, int is_arith)
{
    int mask = (ot == MO_64 ? 0x3f : 0x1f);

    /* load */
    if (op1 == OR_TMP0)
        gen_op_ld_v(s, ot, cpu_T0, cpu_A0);
    else
        gen_op_mov_v_reg(ot, cpu_T0, op1);

    op2 &= mask;
    if (op2 != 0) {
        if (is_right) {
            if (is_arith) {
                gen_exts(ot, cpu_T0);
                tcg_gen_sari_tl(cpu_tmp4, cpu_T0, op2 - 1);
                tcg_gen_sari_tl(cpu_T0, cpu_T0, op2);
            } else {
                gen_extu(ot, cpu_T0);
                tcg_gen_shri_tl(cpu_tmp4, cpu_T0, op2 - 1);
                tcg_gen_shri_tl(cpu_T0, cpu_T0, op2);
            }
        } else {
            tcg_gen_shli_tl(cpu_tmp4, cpu_T0, op2 - 1);
            tcg_gen_shli_tl(cpu_T0, cpu_T0, op2);
        }
    }

    /* store */
    gen_op_st_rm_T0_A0(s, ot, op1);

    /* update eflags if non zero shift */
    if (op2 != 0) {
        tcg_gen_mov_tl(cpu_cc_src, cpu_tmp4);
        tcg_gen_mov_tl(cpu_cc_dst, cpu_T0);
        set_cc_op(s, (is_right ? CC_OP_SARB : CC_OP_SHLB) + ot);
    }
}