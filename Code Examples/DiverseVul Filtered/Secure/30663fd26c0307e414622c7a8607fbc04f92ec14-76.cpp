static void gen_rotc_rm_T1(DisasContext *s, TCGMemOp ot, int op1,
                           int is_right)
{
    gen_compute_eflags(s);
    assert(s->cc_op == CC_OP_EFLAGS);

    /* load */
    if (op1 == OR_TMP0)
        gen_op_ld_v(s, ot, cpu_T0, cpu_A0);
    else
        gen_op_mov_v_reg(ot, cpu_T0, op1);
    
    if (is_right) {
        switch (ot) {
        case MO_8:
            gen_helper_rcrb(cpu_T0, cpu_env, cpu_T0, cpu_T1);
            break;
        case MO_16:
            gen_helper_rcrw(cpu_T0, cpu_env, cpu_T0, cpu_T1);
            break;
        case MO_32:
            gen_helper_rcrl(cpu_T0, cpu_env, cpu_T0, cpu_T1);
            break;
#ifdef TARGET_X86_64
        case MO_64:
            gen_helper_rcrq(cpu_T0, cpu_env, cpu_T0, cpu_T1);
            break;
#endif
        default:
            tcg_abort();
        }
    } else {
        switch (ot) {
        case MO_8:
            gen_helper_rclb(cpu_T0, cpu_env, cpu_T0, cpu_T1);
            break;
        case MO_16:
            gen_helper_rclw(cpu_T0, cpu_env, cpu_T0, cpu_T1);
            break;
        case MO_32:
            gen_helper_rcll(cpu_T0, cpu_env, cpu_T0, cpu_T1);
            break;
#ifdef TARGET_X86_64
        case MO_64:
            gen_helper_rclq(cpu_T0, cpu_env, cpu_T0, cpu_T1);
            break;
#endif
        default:
            tcg_abort();
        }
    }
    /* store */
    gen_op_st_rm_T0_A0(s, ot, op1);
}