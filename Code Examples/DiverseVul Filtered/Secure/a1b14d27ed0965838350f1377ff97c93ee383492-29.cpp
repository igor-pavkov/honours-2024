static int check_mem_access(struct verifier_env *env, u32 regno, int off,
			    int bpf_size, enum bpf_access_type t,
			    int value_regno)
{
	struct verifier_state *state = &env->cur_state;
	int size, err = 0;

	if (state->regs[regno].type == PTR_TO_STACK)
		off += state->regs[regno].imm;

	size = bpf_size_to_bytes(bpf_size);
	if (size < 0)
		return size;

	if (off % size != 0) {
		verbose("misaligned access off %d size %d\n", off, size);
		return -EACCES;
	}

	if (state->regs[regno].type == PTR_TO_MAP_VALUE) {
		if (t == BPF_WRITE && value_regno >= 0 &&
		    is_pointer_value(env, value_regno)) {
			verbose("R%d leaks addr into map\n", value_regno);
			return -EACCES;
		}
		err = check_map_access(env, regno, off, size);
		if (!err && t == BPF_READ && value_regno >= 0)
			mark_reg_unknown_value(state->regs, value_regno);

	} else if (state->regs[regno].type == PTR_TO_CTX) {
		if (t == BPF_WRITE && value_regno >= 0 &&
		    is_pointer_value(env, value_regno)) {
			verbose("R%d leaks addr into ctx\n", value_regno);
			return -EACCES;
		}
		err = check_ctx_access(env, off, size, t);
		if (!err && t == BPF_READ && value_regno >= 0)
			mark_reg_unknown_value(state->regs, value_regno);

	} else if (state->regs[regno].type == FRAME_PTR ||
		   state->regs[regno].type == PTR_TO_STACK) {
		if (off >= 0 || off < -MAX_BPF_STACK) {
			verbose("invalid stack off=%d size=%d\n", off, size);
			return -EACCES;
		}
		if (t == BPF_WRITE) {
			if (!env->allow_ptr_leaks &&
			    state->stack_slot_type[MAX_BPF_STACK + off] == STACK_SPILL &&
			    size != BPF_REG_SIZE) {
				verbose("attempt to corrupt spilled pointer on stack\n");
				return -EACCES;
			}
			err = check_stack_write(state, off, size, value_regno);
		} else {
			err = check_stack_read(state, off, size, value_regno);
		}
	} else {
		verbose("R%d invalid mem access '%s'\n",
			regno, reg_type_str[state->regs[regno].type]);
		return -EACCES;
	}
	return err;
}