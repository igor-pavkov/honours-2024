static int mark_reg_read(struct bpf_verifier_env *env,
			 const struct bpf_verifier_state *state,
			 struct bpf_verifier_state *parent,
			 u32 regno)
{
	bool writes = parent == state->parent; /* Observe write marks */

	if (regno == BPF_REG_FP)
		/* We don't need to worry about FP liveness because it's read-only */
		return 0;

	while (parent) {
		/* if read wasn't screened by an earlier write ... */
		if (writes && state->frame[state->curframe]->regs[regno].live & REG_LIVE_WRITTEN)
			break;
		parent = skip_callee(env, state, parent, regno);
		if (!parent)
			return -EFAULT;
		/* ... then we depend on parent's value */
		parent->frame[parent->curframe]->regs[regno].live |= REG_LIVE_READ;
		state = parent;
		parent = state->parent;
		writes = true;
	}
	return 0;
}