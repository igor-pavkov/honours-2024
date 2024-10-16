static int propagate_liveness(struct bpf_verifier_env *env,
			      const struct bpf_verifier_state *vstate,
			      struct bpf_verifier_state *vparent)
{
	int i, frame, err = 0;
	struct bpf_func_state *state, *parent;

	if (vparent->curframe != vstate->curframe) {
		WARN(1, "propagate_live: parent frame %d current frame %d\n",
		     vparent->curframe, vstate->curframe);
		return -EFAULT;
	}
	/* Propagate read liveness of registers... */
	BUILD_BUG_ON(BPF_REG_FP + 1 != MAX_BPF_REG);
	/* We don't need to worry about FP liveness because it's read-only */
	for (i = 0; i < BPF_REG_FP; i++) {
		if (vparent->frame[vparent->curframe]->regs[i].live & REG_LIVE_READ)
			continue;
		if (vstate->frame[vstate->curframe]->regs[i].live & REG_LIVE_READ) {
			err = mark_reg_read(env, vstate, vparent, i);
			if (err)
				return err;
		}
	}

	/* ... and stack slots */
	for (frame = 0; frame <= vstate->curframe; frame++) {
		state = vstate->frame[frame];
		parent = vparent->frame[frame];
		for (i = 0; i < state->allocated_stack / BPF_REG_SIZE &&
			    i < parent->allocated_stack / BPF_REG_SIZE; i++) {
			if (parent->stack[i].spilled_ptr.live & REG_LIVE_READ)
				continue;
			if (state->stack[i].spilled_ptr.live & REG_LIVE_READ)
				mark_stack_slot_read(env, vstate, vparent, i, frame);
		}
	}
	return err;
}