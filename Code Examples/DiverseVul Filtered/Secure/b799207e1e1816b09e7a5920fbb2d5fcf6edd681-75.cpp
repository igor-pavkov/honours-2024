static void mark_map_regs(struct bpf_verifier_state *vstate, u32 regno,
			  bool is_null)
{
	struct bpf_func_state *state = vstate->frame[vstate->curframe];
	struct bpf_reg_state *regs = state->regs;
	u32 id = regs[regno].id;
	int i, j;

	for (i = 0; i < MAX_BPF_REG; i++)
		mark_map_reg(regs, i, id, is_null);

	for (j = 0; j <= vstate->curframe; j++) {
		state = vstate->frame[j];
		for (i = 0; i < state->allocated_stack / BPF_REG_SIZE; i++) {
			if (state->stack[i].slot_type[0] != STACK_SPILL)
				continue;
			mark_map_reg(&state->stack[i].spilled_ptr, 0, id, is_null);
		}
	}
}