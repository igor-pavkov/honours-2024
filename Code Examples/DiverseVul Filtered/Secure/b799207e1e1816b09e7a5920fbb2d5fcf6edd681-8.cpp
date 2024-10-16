static int check_stack_write(struct bpf_verifier_env *env,
			     struct bpf_func_state *state, /* func where register points to */
			     int off, int size, int value_regno, int insn_idx)
{
	struct bpf_func_state *cur; /* state of the current function */
	int i, slot = -off - 1, spi = slot / BPF_REG_SIZE, err;
	enum bpf_reg_type type;

	err = realloc_func_state(state, round_up(slot + 1, BPF_REG_SIZE),
				 true);
	if (err)
		return err;
	/* caller checked that off % size == 0 and -MAX_BPF_STACK <= off < 0,
	 * so it's aligned access and [off, off + size) are within stack limits
	 */
	if (!env->allow_ptr_leaks &&
	    state->stack[spi].slot_type[0] == STACK_SPILL &&
	    size != BPF_REG_SIZE) {
		verbose(env, "attempt to corrupt spilled pointer on stack\n");
		return -EACCES;
	}

	cur = env->cur_state->frame[env->cur_state->curframe];
	if (value_regno >= 0 &&
	    is_spillable_regtype((type = cur->regs[value_regno].type))) {

		/* register containing pointer is being spilled into stack */
		if (size != BPF_REG_SIZE) {
			verbose(env, "invalid size of register spill\n");
			return -EACCES;
		}

		if (state != cur && type == PTR_TO_STACK) {
			verbose(env, "cannot spill pointers to stack into stack frame of the caller\n");
			return -EINVAL;
		}

		/* save register state */
		state->stack[spi].spilled_ptr = cur->regs[value_regno];
		state->stack[spi].spilled_ptr.live |= REG_LIVE_WRITTEN;

		for (i = 0; i < BPF_REG_SIZE; i++) {
			if (state->stack[spi].slot_type[i] == STACK_MISC &&
			    !env->allow_ptr_leaks) {
				int *poff = &env->insn_aux_data[insn_idx].sanitize_stack_off;
				int soff = (-spi - 1) * BPF_REG_SIZE;

				/* detected reuse of integer stack slot with a pointer
				 * which means either llvm is reusing stack slot or
				 * an attacker is trying to exploit CVE-2018-3639
				 * (speculative store bypass)
				 * Have to sanitize that slot with preemptive
				 * store of zero.
				 */
				if (*poff && *poff != soff) {
					/* disallow programs where single insn stores
					 * into two different stack slots, since verifier
					 * cannot sanitize them
					 */
					verbose(env,
						"insn %d cannot access two stack slots fp%d and fp%d",
						insn_idx, *poff, soff);
					return -EINVAL;
				}
				*poff = soff;
			}
			state->stack[spi].slot_type[i] = STACK_SPILL;
		}
	} else {
		u8 type = STACK_MISC;

		/* regular write of data into stack */
		state->stack[spi].spilled_ptr = (struct bpf_reg_state) {};

		/* only mark the slot as written if all 8 bytes were written
		 * otherwise read propagation may incorrectly stop too soon
		 * when stack slots are partially written.
		 * This heuristic means that read propagation will be
		 * conservative, since it will add reg_live_read marks
		 * to stack slots all the way to first state when programs
		 * writes+reads less than 8 bytes
		 */
		if (size == BPF_REG_SIZE)
			state->stack[spi].spilled_ptr.live |= REG_LIVE_WRITTEN;

		/* when we zero initialize stack slots mark them as such */
		if (value_regno >= 0 &&
		    register_is_null(&cur->regs[value_regno]))
			type = STACK_ZERO;

		for (i = 0; i < size; i++)
			state->stack[spi].slot_type[(slot - i) % BPF_REG_SIZE] =
				type;
	}
	return 0;
}