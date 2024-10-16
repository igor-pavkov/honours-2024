static void gen_rot_rm_im(DisasContext *s, TCGMemOp ot, int op1, int op2,
                          int is_right)
{
    int mask = (ot == MO_64 ? 0x3f : 0x1f);
    int shift;

    /* load */
    if (op1 == OR_TMP0) {
        gen_op_ld_v(s, ot, cpu_T0, cpu_A0);
    } else {
        gen_op_mov_v_reg(ot, cpu_T0, op1);
    }

    op2 &= mask;
    if (op2 != 0) {
        switch (ot) {
#ifdef TARGET_X86_64
        case MO_32:
            tcg_gen_trunc_tl_i32(cpu_tmp2_i32, cpu_T0);
            if (is_right) {
                tcg_gen_rotri_i32(cpu_tmp2_i32, cpu_tmp2_i32, op2);
            } else {
                tcg_gen_rotli_i32(cpu_tmp2_i32, cpu_tmp2_i32, op2);
            }
            tcg_gen_extu_i32_tl(cpu_T0, cpu_tmp2_i32);
            break;
#endif
        default:
            if (is_right) {
                tcg_gen_rotri_tl(cpu_T0, cpu_T0, op2);
            } else {
                tcg_gen_rotli_tl(cpu_T0, cpu_T0, op2);
            }
            break;
        case MO_8:
            mask = 7;
            goto do_shifts;
        case MO_16:
            mask = 15;
        do_shifts:
            shift = op2 & mask;
            if (is_right) {
                shift = mask + 1 - shift;
            }
            gen_extu(ot, cpu_T0);
            tcg_gen_shli_tl(cpu_tmp0, cpu_T0, shift);
            tcg_gen_shri_tl(cpu_T0, cpu_T0, mask + 1 - shift);
            tcg_gen_or_tl(cpu_T0, cpu_T0, cpu_tmp0);
            break;
        }
    }

    /* store */
    gen_op_st_rm_T0_A0(s, ot, op1);

    if (op2 != 0) {
        /* Compute the flags into CC_SRC.  */
        gen_compute_eflags(s);

        /* The value that was "rotated out" is now present at the other end
           of the word.  Compute C into CC_DST and O into CC_SRC2.  Note that
           since we've computed the flags into CC_SRC, these variables are
           currently dead.  */
        if (is_right) {
            tcg_gen_shri_tl(cpu_cc_src2, cpu_T0, mask - 1);
            tcg_gen_shri_tl(cpu_cc_dst, cpu_T0, mask);
            tcg_gen_andi_tl(cpu_cc_dst, cpu_cc_dst, 1);
        } else {
            tcg_gen_shri_tl(cpu_cc_src2, cpu_T0, mask);
            tcg_gen_andi_tl(cpu_cc_dst, cpu_T0, 1);
        }
        tcg_gen_andi_tl(cpu_cc_src2, cpu_cc_src2, 1);
        tcg_gen_xor_tl(cpu_cc_src2, cpu_cc_src2, cpu_cc_dst);
        set_cc_op(s, CC_OP_ADCOX);
    }
}