static void print_verifier_state(struct verifier_env *env)
{
	enum bpf_reg_type t;
	int i;

	for (i = 0; i < MAX_BPF_REG; i++) {
		t = env->cur_state.regs[i].type;
		if (t == NOT_INIT)
			continue;
		verbose(" R%d=%s", i, reg_type_str[t]);
		if (t == CONST_IMM || t == PTR_TO_STACK)
			verbose("%d", env->cur_state.regs[i].imm);
		else if (t == CONST_PTR_TO_MAP || t == PTR_TO_MAP_VALUE ||
			 t == PTR_TO_MAP_VALUE_OR_NULL)
			verbose("(ks=%d,vs=%d)",
				env->cur_state.regs[i].map_ptr->key_size,
				env->cur_state.regs[i].map_ptr->value_size);
	}
	for (i = 0; i < MAX_BPF_STACK; i += BPF_REG_SIZE) {
		if (env->cur_state.stack_slot_type[i] == STACK_SPILL)
			verbose(" fp%d=%s", -MAX_BPF_STACK + i,
				reg_type_str[env->cur_state.spilled_regs[i / BPF_REG_SIZE].type]);
	}
	verbose("\n");
}