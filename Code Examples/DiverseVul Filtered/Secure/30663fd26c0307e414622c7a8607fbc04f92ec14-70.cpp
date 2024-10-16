static void gen_illegal_opcode(DisasContext *s)
{
    gen_exception(s, EXCP06_ILLOP, s->pc_start - s->cs_base);
}