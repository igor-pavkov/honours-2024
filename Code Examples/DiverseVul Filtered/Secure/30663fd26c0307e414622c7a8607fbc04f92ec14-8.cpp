static void gen_shift_rm_T1(DisasContext *s, TCGMemOp ot, int op1,
                            int is_right, int is_arith)
{
    target_ulong mask = (ot == MO_64 ? 0x3f : 0x1f);

    /* load */
    if (op1 == OR_TMP0) {
        gen_op_ld_v(s, ot, cpu_T0, cpu_A0);
    } else {
        gen_op_mov_v_reg(ot, cpu_T0, op1);
    }

    tcg_gen_andi_tl(cpu_T1, cpu_T1, mask);
    tcg_gen_subi_tl(cpu_tmp0, cpu_T1, 1);

    if (is_right) {
        if (is_arith) {
            gen_exts(ot, cpu_T0);
            tcg_gen_sar_tl(cpu_tmp0, cpu_T0, cpu_tmp0);
            tcg_gen_sar_tl(cpu_T0, cpu_T0, cpu_T1);
        } else {
            gen_extu(ot, cpu_T0);
            tcg_gen_shr_tl(cpu_tmp0, cpu_T0, cpu_tmp0);
            tcg_gen_shr_tl(cpu_T0, cpu_T0, cpu_T1);
        }
    } else {
        tcg_gen_shl_tl(cpu_tmp0, cpu_T0, cpu_tmp0);
        tcg_gen_shl_tl(cpu_T0, cpu_T0, cpu_T1);
    }

    /* store */
    gen_op_st_rm_T0_A0(s, ot, op1);

    gen_shift_flags(s, ot, cpu_T0, cpu_tmp0, cpu_T1, is_right);
}