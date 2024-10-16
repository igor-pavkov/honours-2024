static int check_func_proto(const struct bpf_func_proto *fn)
{
	return check_raw_mode_ok(fn) &&
	       check_arg_pair_ok(fn) ? 0 : -EINVAL;
}