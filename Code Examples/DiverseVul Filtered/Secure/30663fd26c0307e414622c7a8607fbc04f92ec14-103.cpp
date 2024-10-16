static inline void gen_stack_A0(DisasContext *s)
{
    gen_lea_v_seg(s, s->ss32 ? MO_32 : MO_16, cpu_regs[R_ESP], R_SS, -1);
}