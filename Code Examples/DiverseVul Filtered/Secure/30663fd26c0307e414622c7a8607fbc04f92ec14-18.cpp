static void gen_add_A0_im(DisasContext *s, int val)
{
    tcg_gen_addi_tl(cpu_A0, cpu_A0, val);
    if (!CODE64(s)) {
        tcg_gen_ext32u_tl(cpu_A0, cpu_A0);
    }
}