static void gen_push_v(DisasContext *s, TCGv val)
{
    TCGMemOp d_ot = mo_pushpop(s, s->dflag);
    TCGMemOp a_ot = mo_stacksize(s);
    int size = 1 << d_ot;
    TCGv new_esp = cpu_A0;

    tcg_gen_subi_tl(cpu_A0, cpu_regs[R_ESP], size);

    if (!CODE64(s)) {
        if (s->addseg) {
            new_esp = cpu_tmp4;
            tcg_gen_mov_tl(new_esp, cpu_A0);
        }
        gen_lea_v_seg(s, a_ot, cpu_A0, R_SS, -1);
    }

    gen_op_st_v(s, d_ot, val, cpu_A0);
    gen_op_mov_reg_v(a_ot, R_ESP, new_esp);
}