static int realloc_func_state(struct bpf_func_state *state, int size,
			      bool copy_old)
{
	u32 old_size = state->allocated_stack;
	struct bpf_stack_state *new_stack;
	int slot = size / BPF_REG_SIZE;

	if (size <= old_size || !size) {
		if (copy_old)
			return 0;
		state->allocated_stack = slot * BPF_REG_SIZE;
		if (!size && old_size) {
			kfree(state->stack);
			state->stack = NULL;
		}
		return 0;
	}
	new_stack = kmalloc_array(slot, sizeof(struct bpf_stack_state),
				  GFP_KERNEL);
	if (!new_stack)
		return -ENOMEM;
	if (copy_old) {
		if (state->stack)
			memcpy(new_stack, state->stack,
			       sizeof(*new_stack) * (old_size / BPF_REG_SIZE));
		memset(new_stack + old_size / BPF_REG_SIZE, 0,
		       sizeof(*new_stack) * (size - old_size) / BPF_REG_SIZE);
	}
	state->allocated_stack = slot * BPF_REG_SIZE;
	kfree(state->stack);
	state->stack = new_stack;
	return 0;
}