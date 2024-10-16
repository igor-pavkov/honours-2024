static inline void gen_pop_update(DisasContext *s, TCGMemOp ot)
{
    gen_stack_update(s, 1 << ot);
}