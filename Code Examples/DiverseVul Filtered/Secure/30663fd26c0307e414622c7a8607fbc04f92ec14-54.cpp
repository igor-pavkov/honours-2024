static void gen_popa(DisasContext *s)
{
    TCGMemOp s_ot = s->ss32 ? MO_32 : MO_16;
    TCGMemOp d_ot = s->dflag;
    int size = 1 << d_ot;
    int i;

    for (i = 0; i < 8; i++) {
        /* ESP is not reloaded */
        if (7 - i == R_ESP) {
            continue;
        }
        tcg_gen_addi_tl(cpu_A0, cpu_regs[R_ESP], i * size);
        gen_lea_v_seg(s, s_ot, cpu_A0, R_SS, -1);
        gen_op_ld_v(s, d_ot, cpu_T0, cpu_A0);
        gen_op_mov_reg_v(d_ot, 7 - i, cpu_T0);
    }

    gen_stack_update(s, 8 * size);
}