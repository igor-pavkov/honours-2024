static inline void gen_stack_update(DisasContext *s, int addend)
{
    gen_op_add_reg_im(mo_stacksize(s), R_ESP, addend);
}