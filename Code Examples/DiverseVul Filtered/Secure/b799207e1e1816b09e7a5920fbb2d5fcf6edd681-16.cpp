static void __mark_reg_unknown(struct bpf_reg_state *reg)
{
	reg->type = SCALAR_VALUE;
	reg->id = 0;
	reg->off = 0;
	reg->var_off = tnum_unknown;
	reg->frameno = 0;
	__mark_reg_unbounded(reg);
}