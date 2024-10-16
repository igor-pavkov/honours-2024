static void do_refine_retval_range(struct bpf_reg_state *regs, int ret_type,
				   int func_id,
				   struct bpf_call_arg_meta *meta)
{
	struct bpf_reg_state *ret_reg = &regs[BPF_REG_0];

	if (ret_type != RET_INTEGER ||
	    (func_id != BPF_FUNC_get_stack &&
	     func_id != BPF_FUNC_probe_read_str))
		return;

	ret_reg->smax_value = meta->msize_smax_value;
	ret_reg->umax_value = meta->msize_umax_value;
	__reg_deduce_bounds(ret_reg);
	__reg_bound_offset(ret_reg);
}