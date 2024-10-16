static void gen_op(DisasContext *s1, int op, TCGMemOp ot, int d)
{
    if (d != OR_TMP0) {
        gen_op_mov_v_reg(ot, cpu_T0, d);
    } else if (!(s1->prefix & PREFIX_LOCK)) {
        gen_op_ld_v(s1, ot, cpu_T0, cpu_A0);
    }
    switch(op) {
    case OP_ADCL:
        gen_compute_eflags_c(s1, cpu_tmp4);
        if (s1->prefix & PREFIX_LOCK) {
            tcg_gen_add_tl(cpu_T0, cpu_tmp4, cpu_T1);
            tcg_gen_atomic_add_fetch_tl(cpu_T0, cpu_A0, cpu_T0,
                                        s1->mem_index, ot | MO_LE);
        } else {
            tcg_gen_add_tl(cpu_T0, cpu_T0, cpu_T1);
            tcg_gen_add_tl(cpu_T0, cpu_T0, cpu_tmp4);
            gen_op_st_rm_T0_A0(s1, ot, d);
        }
        gen_op_update3_cc(cpu_tmp4);
        set_cc_op(s1, CC_OP_ADCB + ot);
        break;
    case OP_SBBL:
        gen_compute_eflags_c(s1, cpu_tmp4);
        if (s1->prefix & PREFIX_LOCK) {
            tcg_gen_add_tl(cpu_T0, cpu_T1, cpu_tmp4);
            tcg_gen_neg_tl(cpu_T0, cpu_T0);
            tcg_gen_atomic_add_fetch_tl(cpu_T0, cpu_A0, cpu_T0,
                                        s1->mem_index, ot | MO_LE);
        } else {
            tcg_gen_sub_tl(cpu_T0, cpu_T0, cpu_T1);
            tcg_gen_sub_tl(cpu_T0, cpu_T0, cpu_tmp4);
            gen_op_st_rm_T0_A0(s1, ot, d);
        }
        gen_op_update3_cc(cpu_tmp4);
        set_cc_op(s1, CC_OP_SBBB + ot);
        break;
    case OP_ADDL:
        if (s1->prefix & PREFIX_LOCK) {
            tcg_gen_atomic_add_fetch_tl(cpu_T0, cpu_A0, cpu_T1,
                                        s1->mem_index, ot | MO_LE);
        } else {
            tcg_gen_add_tl(cpu_T0, cpu_T0, cpu_T1);
            gen_op_st_rm_T0_A0(s1, ot, d);
        }
        gen_op_update2_cc();
        set_cc_op(s1, CC_OP_ADDB + ot);
        break;
    case OP_SUBL:
        if (s1->prefix & PREFIX_LOCK) {
            tcg_gen_neg_tl(cpu_T0, cpu_T1);
            tcg_gen_atomic_fetch_add_tl(cpu_cc_srcT, cpu_A0, cpu_T0,
                                        s1->mem_index, ot | MO_LE);
            tcg_gen_sub_tl(cpu_T0, cpu_cc_srcT, cpu_T1);
        } else {
            tcg_gen_mov_tl(cpu_cc_srcT, cpu_T0);
            tcg_gen_sub_tl(cpu_T0, cpu_T0, cpu_T1);
            gen_op_st_rm_T0_A0(s1, ot, d);
        }
        gen_op_update2_cc();
        set_cc_op(s1, CC_OP_SUBB + ot);
        break;
    default:
    case OP_ANDL:
        if (s1->prefix & PREFIX_LOCK) {
            tcg_gen_atomic_and_fetch_tl(cpu_T0, cpu_A0, cpu_T1,
                                        s1->mem_index, ot | MO_LE);
        } else {
            tcg_gen_and_tl(cpu_T0, cpu_T0, cpu_T1);
            gen_op_st_rm_T0_A0(s1, ot, d);
        }
        gen_op_update1_cc();
        set_cc_op(s1, CC_OP_LOGICB + ot);
        break;
    case OP_ORL:
        if (s1->prefix & PREFIX_LOCK) {
            tcg_gen_atomic_or_fetch_tl(cpu_T0, cpu_A0, cpu_T1,
                                       s1->mem_index, ot | MO_LE);
        } else {
            tcg_gen_or_tl(cpu_T0, cpu_T0, cpu_T1);
            gen_op_st_rm_T0_A0(s1, ot, d);
        }
        gen_op_update1_cc();
        set_cc_op(s1, CC_OP_LOGICB + ot);
        break;
    case OP_XORL:
        if (s1->prefix & PREFIX_LOCK) {
            tcg_gen_atomic_xor_fetch_tl(cpu_T0, cpu_A0, cpu_T1,
                                        s1->mem_index, ot | MO_LE);
        } else {
            tcg_gen_xor_tl(cpu_T0, cpu_T0, cpu_T1);
            gen_op_st_rm_T0_A0(s1, ot, d);
        }
        gen_op_update1_cc();
        set_cc_op(s1, CC_OP_LOGICB + ot);
        break;
    case OP_CMPL:
        tcg_gen_mov_tl(cpu_cc_src, cpu_T1);
        tcg_gen_mov_tl(cpu_cc_srcT, cpu_T0);
        tcg_gen_sub_tl(cpu_cc_dst, cpu_T0, cpu_T1);
        set_cc_op(s1, CC_OP_SUBB + ot);
        break;
    }
}