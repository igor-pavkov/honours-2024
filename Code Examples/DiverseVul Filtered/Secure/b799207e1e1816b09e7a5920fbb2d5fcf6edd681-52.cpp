static int copy_func_state(struct bpf_func_state *dst,
			   const struct bpf_func_state *src)
{
	int err;

	err = realloc_func_state(dst, src->allocated_stack, false);
	if (err)
		return err;
	memcpy(dst, src, offsetof(struct bpf_func_state, allocated_stack));
	return copy_stack_state(dst, src);
}