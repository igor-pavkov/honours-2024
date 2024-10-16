static void gen_check_io(DisasContext *s, TCGMemOp ot, target_ulong cur_eip,
                         uint32_t svm_flags)
{
    target_ulong next_eip;

    if (s->pe && (s->cpl > s->iopl || s->vm86)) {
        tcg_gen_trunc_tl_i32(cpu_tmp2_i32, cpu_T0);
        switch (ot) {
        case MO_8:
            gen_helper_check_iob(cpu_env, cpu_tmp2_i32);
            break;
        case MO_16:
            gen_helper_check_iow(cpu_env, cpu_tmp2_i32);
            break;
        case MO_32:
            gen_helper_check_iol(cpu_env, cpu_tmp2_i32);
            break;
        default:
            tcg_abort();
        }
    }
    if(s->flags & HF_SVMI_MASK) {
        gen_update_cc_op(s);
        gen_jmp_im(cur_eip);
        svm_flags |= (1 << (4 + ot));
        next_eip = s->pc - s->cs_base;
        tcg_gen_trunc_tl_i32(cpu_tmp2_i32, cpu_T0);
        gen_helper_svm_check_io(cpu_env, cpu_tmp2_i32,
                                tcg_const_i32(svm_flags),
                                tcg_const_i32(next_eip - cur_eip));
    }
}