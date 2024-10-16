static void mark_stack_slot_read(struct bpf_verifier_env *env,
				 const struct bpf_verifier_state *state,
				 struct bpf_verifier_state *parent,
				 int slot, int frameno)
{
	bool writes = parent == state->parent; /* Observe write marks */

	while (parent) {
		if (parent->frame[frameno]->allocated_stack <= slot * BPF_REG_SIZE)
			/* since LIVE_WRITTEN mark is only done for full 8-byte
			 * write the read marks are conservative and parent
			 * state may not even have the stack allocated. In such case
			 * end the propagation, since the loop reached beginning
			 * of the function
			 */
			break;
		/* if read wasn't screened by an earlier write ... */
		if (writes && state->frame[frameno]->stack[slot].spilled_ptr.live & REG_LIVE_WRITTEN)
			break;
		/* ... then we depend on parent's value */
		parent->frame[frameno]->stack[slot].spilled_ptr.live |= REG_LIVE_READ;
		state = parent;
		parent = state->parent;
		writes = true;
	}
}