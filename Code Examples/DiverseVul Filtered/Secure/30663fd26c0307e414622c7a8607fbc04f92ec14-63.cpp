static void gen_pusha(DisasContext *s)
{
    TCGMemOp s_ot = s->ss32 ? MO_32 : MO_16;
    TCGMemOp d_ot = s->dflag;
    int size = 1 << d_ot;
    int i;

    for (i = 0; i < 8; i++) {
        tcg_gen_addi_tl(cpu_A0, cpu_regs[R_ESP], (i - 8) * size);
        gen_lea_v_seg(s, s_ot, cpu_A0, R_SS, -1);
        gen_op_st_v(s, d_ot, cpu_regs[7 - i], cpu_A0);
    }

    gen_stack_update(s, -8 * size);
}