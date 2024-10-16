static void gen_enter(DisasContext *s, int esp_addend, int level)
{
    TCGMemOp d_ot = mo_pushpop(s, s->dflag);
    TCGMemOp a_ot = CODE64(s) ? MO_64 : s->ss32 ? MO_32 : MO_16;
    int size = 1 << d_ot;

    /* Push BP; compute FrameTemp into T1.  */
    tcg_gen_subi_tl(cpu_T1, cpu_regs[R_ESP], size);
    gen_lea_v_seg(s, a_ot, cpu_T1, R_SS, -1);
    gen_op_st_v(s, d_ot, cpu_regs[R_EBP], cpu_A0);

    level &= 31;
    if (level != 0) {
        int i;

        /* Copy level-1 pointers from the previous frame.  */
        for (i = 1; i < level; ++i) {
            tcg_gen_subi_tl(cpu_A0, cpu_regs[R_EBP], size * i);
            gen_lea_v_seg(s, a_ot, cpu_A0, R_SS, -1);
            gen_op_ld_v(s, d_ot, cpu_tmp0, cpu_A0);

            tcg_gen_subi_tl(cpu_A0, cpu_T1, size * i);
            gen_lea_v_seg(s, a_ot, cpu_A0, R_SS, -1);
            gen_op_st_v(s, d_ot, cpu_tmp0, cpu_A0);
        }

        /* Push the current FrameTemp as the last level.  */
        tcg_gen_subi_tl(cpu_A0, cpu_T1, size * level);
        gen_lea_v_seg(s, a_ot, cpu_A0, R_SS, -1);
        gen_op_st_v(s, d_ot, cpu_T1, cpu_A0);
    }

    /* Copy the FrameTemp value to EBP.  */
    gen_op_mov_reg_v(a_ot, R_EBP, cpu_T1);

    /* Compute the final value of ESP.  */
    tcg_gen_subi_tl(cpu_T1, cpu_T1, esp_addend + size * level);
    gen_op_mov_reg_v(a_ot, R_ESP, cpu_T1);
}