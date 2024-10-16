static bool is_pkt_reg(struct bpf_verifier_env *env, int regno)
{
	const struct bpf_reg_state *reg = cur_regs(env) + regno;

	return type_is_pkt_pointer(reg->type);
}