static int check_call(struct verifier_env *env, int func_id)
{
	struct verifier_state *state = &env->cur_state;
	const struct bpf_func_proto *fn = NULL;
	struct reg_state *regs = state->regs;
	struct bpf_map *map = NULL;
	struct reg_state *reg;
	int i, err;

	/* find function prototype */
	if (func_id < 0 || func_id >= __BPF_FUNC_MAX_ID) {
		verbose("invalid func %d\n", func_id);
		return -EINVAL;
	}

	if (env->prog->aux->ops->get_func_proto)
		fn = env->prog->aux->ops->get_func_proto(func_id);

	if (!fn) {
		verbose("unknown func %d\n", func_id);
		return -EINVAL;
	}

	/* eBPF programs must be GPL compatible to use GPL-ed functions */
	if (!env->prog->gpl_compatible && fn->gpl_only) {
		verbose("cannot call GPL only function from proprietary program\n");
		return -EINVAL;
	}

	/* check args */
	err = check_func_arg(env, BPF_REG_1, fn->arg1_type, &map);
	if (err)
		return err;
	err = check_func_arg(env, BPF_REG_2, fn->arg2_type, &map);
	if (err)
		return err;
	err = check_func_arg(env, BPF_REG_3, fn->arg3_type, &map);
	if (err)
		return err;
	err = check_func_arg(env, BPF_REG_4, fn->arg4_type, &map);
	if (err)
		return err;
	err = check_func_arg(env, BPF_REG_5, fn->arg5_type, &map);
	if (err)
		return err;

	/* reset caller saved regs */
	for (i = 0; i < CALLER_SAVED_REGS; i++) {
		reg = regs + caller_saved[i];
		reg->type = NOT_INIT;
		reg->imm = 0;
	}

	/* update return register */
	if (fn->ret_type == RET_INTEGER) {
		regs[BPF_REG_0].type = UNKNOWN_VALUE;
	} else if (fn->ret_type == RET_VOID) {
		regs[BPF_REG_0].type = NOT_INIT;
	} else if (fn->ret_type == RET_PTR_TO_MAP_VALUE_OR_NULL) {
		regs[BPF_REG_0].type = PTR_TO_MAP_VALUE_OR_NULL;
		/* remember map_ptr, so that check_map_access()
		 * can check 'value_size' boundary of memory access
		 * to map element returned from bpf_map_lookup_elem()
		 */
		if (map == NULL) {
			verbose("kernel subsystem misconfigured verifier\n");
			return -EINVAL;
		}
		regs[BPF_REG_0].map_ptr = map;
	} else {
		verbose("unknown return type %d of func %d\n",
			fn->ret_type, func_id);
		return -EINVAL;
	}

	err = check_map_func_compatibility(map, func_id);
	if (err)
		return err;

	return 0;
}