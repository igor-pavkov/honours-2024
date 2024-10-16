static void gen_shiftd_rm_T1(DisasContext *s, TCGMemOp ot, int op1,
                             bool is_right, TCGv count_in)
{
    target_ulong mask = (ot == MO_64 ? 63 : 31);
    TCGv count;

    /* load */
    if (op1 == OR_TMP0) {
        gen_op_ld_v(s, ot, cpu_T0, cpu_A0);
    } else {
        gen_op_mov_v_reg(ot, cpu_T0, op1);
    }

    count = tcg_temp_new();
    tcg_gen_andi_tl(count, count_in, mask);

    switch (ot) {
    case MO_16:
        /* Note: we implement the Intel behaviour for shift count > 16.
           This means "shrdw C, B, A" shifts A:B:A >> C.  Build the B:A
           portion by constructing it as a 32-bit value.  */
        if (is_right) {
            tcg_gen_deposit_tl(cpu_tmp0, cpu_T0, cpu_T1, 16, 16);
            tcg_gen_mov_tl(cpu_T1, cpu_T0);
            tcg_gen_mov_tl(cpu_T0, cpu_tmp0);
        } else {
            tcg_gen_deposit_tl(cpu_T1, cpu_T0, cpu_T1, 16, 16);
        }
        /* FALLTHRU */
#ifdef TARGET_X86_64
    case MO_32:
        /* Concatenate the two 32-bit values and use a 64-bit shift.  */
        tcg_gen_subi_tl(cpu_tmp0, count, 1);
        if (is_right) {
            tcg_gen_concat_tl_i64(cpu_T0, cpu_T0, cpu_T1);
            tcg_gen_shr_i64(cpu_tmp0, cpu_T0, cpu_tmp0);
            tcg_gen_shr_i64(cpu_T0, cpu_T0, count);
        } else {
            tcg_gen_concat_tl_i64(cpu_T0, cpu_T1, cpu_T0);
            tcg_gen_shl_i64(cpu_tmp0, cpu_T0, cpu_tmp0);
            tcg_gen_shl_i64(cpu_T0, cpu_T0, count);
            tcg_gen_shri_i64(cpu_tmp0, cpu_tmp0, 32);
            tcg_gen_shri_i64(cpu_T0, cpu_T0, 32);
        }
        break;
#endif
    default:
        tcg_gen_subi_tl(cpu_tmp0, count, 1);
        if (is_right) {
            tcg_gen_shr_tl(cpu_tmp0, cpu_T0, cpu_tmp0);

            tcg_gen_subfi_tl(cpu_tmp4, mask + 1, count);
            tcg_gen_shr_tl(cpu_T0, cpu_T0, count);
            tcg_gen_shl_tl(cpu_T1, cpu_T1, cpu_tmp4);
        } else {
            tcg_gen_shl_tl(cpu_tmp0, cpu_T0, cpu_tmp0);
            if (ot == MO_16) {
                /* Only needed if count > 16, for Intel behaviour.  */
                tcg_gen_subfi_tl(cpu_tmp4, 33, count);
                tcg_gen_shr_tl(cpu_tmp4, cpu_T1, cpu_tmp4);
                tcg_gen_or_tl(cpu_tmp0, cpu_tmp0, cpu_tmp4);
            }

            tcg_gen_subfi_tl(cpu_tmp4, mask + 1, count);
            tcg_gen_shl_tl(cpu_T0, cpu_T0, count);
            tcg_gen_shr_tl(cpu_T1, cpu_T1, cpu_tmp4);
        }
        tcg_gen_movi_tl(cpu_tmp4, 0);
        tcg_gen_movcond_tl(TCG_COND_EQ, cpu_T1, count, cpu_tmp4,
                           cpu_tmp4, cpu_T1);
        tcg_gen_or_tl(cpu_T0, cpu_T0, cpu_T1);
        break;
    }

    /* store */
    gen_op_st_rm_T0_A0(s, ot, op1);

    gen_shift_flags(s, ot, cpu_T0, cpu_tmp0, count, is_right);
    tcg_temp_free(count);
}