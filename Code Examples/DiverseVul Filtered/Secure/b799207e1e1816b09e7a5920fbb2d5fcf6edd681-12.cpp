static void __mark_reg_const_zero(struct bpf_reg_state *reg)
{
	__mark_reg_known(reg, 0);
	reg->off = 0;
	reg->type = SCALAR_VALUE;
}