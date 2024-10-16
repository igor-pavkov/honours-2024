static void free_func_state(struct bpf_func_state *state)
{
	if (!state)
		return;
	kfree(state->stack);
	kfree(state);
}