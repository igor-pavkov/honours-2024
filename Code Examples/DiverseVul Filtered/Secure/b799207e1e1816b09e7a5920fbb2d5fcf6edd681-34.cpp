static int copy_stack_state(struct bpf_func_state *dst,
			    const struct bpf_func_state *src)
{
	if (!src->stack)
		return 0;
	if (WARN_ON_ONCE(dst->allocated_stack < src->allocated_stack)) {
		/* internal bug, make state invalid to reject the program */
		memset(dst, 0, sizeof(*dst));
		return -EFAULT;
	}
	memcpy(dst->stack, src->stack,
	       sizeof(*src->stack) * (src->allocated_stack / BPF_REG_SIZE));
	return 0;
}