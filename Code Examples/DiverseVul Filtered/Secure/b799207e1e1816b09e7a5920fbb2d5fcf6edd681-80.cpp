static int do_check(struct bpf_verifier_env *env)
{
	struct bpf_verifier_state *state;
	struct bpf_insn *insns = env->prog->insnsi;
	struct bpf_reg_state *regs;
	int insn_cnt = env->prog->len, i;
	int insn_idx, prev_insn_idx = 0;
	int insn_processed = 0;
	bool do_print_state = false;

	state = kzalloc(sizeof(struct bpf_verifier_state), GFP_KERNEL);
	if (!state)
		return -ENOMEM;
	state->curframe = 0;
	state->parent = NULL;
	state->frame[0] = kzalloc(sizeof(struct bpf_func_state), GFP_KERNEL);
	if (!state->frame[0]) {
		kfree(state);
		return -ENOMEM;
	}
	env->cur_state = state;
	init_func_state(env, state->frame[0],
			BPF_MAIN_FUNC /* callsite */,
			0 /* frameno */,
			0 /* subprogno, zero == main subprog */);
	insn_idx = 0;
	for (;;) {
		struct bpf_insn *insn;
		u8 class;
		int err;

		if (insn_idx >= insn_cnt) {
			verbose(env, "invalid insn idx %d insn_cnt %d\n",
				insn_idx, insn_cnt);
			return -EFAULT;
		}

		insn = &insns[insn_idx];
		class = BPF_CLASS(insn->code);

		if (++insn_processed > BPF_COMPLEXITY_LIMIT_INSNS) {
			verbose(env,
				"BPF program is too large. Processed %d insn\n",
				insn_processed);
			return -E2BIG;
		}

		err = is_state_visited(env, insn_idx);
		if (err < 0)
			return err;
		if (err == 1) {
			/* found equivalent state, can prune the search */
			if (env->log.level) {
				if (do_print_state)
					verbose(env, "\nfrom %d to %d: safe\n",
						prev_insn_idx, insn_idx);
				else
					verbose(env, "%d: safe\n", insn_idx);
			}
			goto process_bpf_exit;
		}

		if (need_resched())
			cond_resched();

		if (env->log.level > 1 || (env->log.level && do_print_state)) {
			if (env->log.level > 1)
				verbose(env, "%d:", insn_idx);
			else
				verbose(env, "\nfrom %d to %d:",
					prev_insn_idx, insn_idx);
			print_verifier_state(env, state->frame[state->curframe]);
			do_print_state = false;
		}

		if (env->log.level) {
			const struct bpf_insn_cbs cbs = {
				.cb_print	= verbose,
				.private_data	= env,
			};

			verbose(env, "%d: ", insn_idx);
			print_bpf_insn(&cbs, insn, env->allow_ptr_leaks);
		}

		if (bpf_prog_is_dev_bound(env->prog->aux)) {
			err = bpf_prog_offload_verify_insn(env, insn_idx,
							   prev_insn_idx);
			if (err)
				return err;
		}

		regs = cur_regs(env);
		env->insn_aux_data[insn_idx].seen = true;
		if (class == BPF_ALU || class == BPF_ALU64) {
			err = check_alu_op(env, insn);
			if (err)
				return err;

		} else if (class == BPF_LDX) {
			enum bpf_reg_type *prev_src_type, src_reg_type;

			/* check for reserved fields is already done */

			/* check src operand */
			err = check_reg_arg(env, insn->src_reg, SRC_OP);
			if (err)
				return err;

			err = check_reg_arg(env, insn->dst_reg, DST_OP_NO_MARK);
			if (err)
				return err;

			src_reg_type = regs[insn->src_reg].type;

			/* check that memory (src_reg + off) is readable,
			 * the state of dst_reg will be updated by this func
			 */
			err = check_mem_access(env, insn_idx, insn->src_reg, insn->off,
					       BPF_SIZE(insn->code), BPF_READ,
					       insn->dst_reg, false);
			if (err)
				return err;

			prev_src_type = &env->insn_aux_data[insn_idx].ptr_type;

			if (*prev_src_type == NOT_INIT) {
				/* saw a valid insn
				 * dst_reg = *(u32 *)(src_reg + off)
				 * save type to validate intersecting paths
				 */
				*prev_src_type = src_reg_type;

			} else if (src_reg_type != *prev_src_type &&
				   (src_reg_type == PTR_TO_CTX ||
				    *prev_src_type == PTR_TO_CTX)) {
				/* ABuser program is trying to use the same insn
				 * dst_reg = *(u32*) (src_reg + off)
				 * with different pointer types:
				 * src_reg == ctx in one branch and
				 * src_reg == stack|map in some other branch.
				 * Reject it.
				 */
				verbose(env, "same insn cannot be used with different pointers\n");
				return -EINVAL;
			}

		} else if (class == BPF_STX) {
			enum bpf_reg_type *prev_dst_type, dst_reg_type;

			if (BPF_MODE(insn->code) == BPF_XADD) {
				err = check_xadd(env, insn_idx, insn);
				if (err)
					return err;
				insn_idx++;
				continue;
			}

			/* check src1 operand */
			err = check_reg_arg(env, insn->src_reg, SRC_OP);
			if (err)
				return err;
			/* check src2 operand */
			err = check_reg_arg(env, insn->dst_reg, SRC_OP);
			if (err)
				return err;

			dst_reg_type = regs[insn->dst_reg].type;

			/* check that memory (dst_reg + off) is writeable */
			err = check_mem_access(env, insn_idx, insn->dst_reg, insn->off,
					       BPF_SIZE(insn->code), BPF_WRITE,
					       insn->src_reg, false);
			if (err)
				return err;

			prev_dst_type = &env->insn_aux_data[insn_idx].ptr_type;

			if (*prev_dst_type == NOT_INIT) {
				*prev_dst_type = dst_reg_type;
			} else if (dst_reg_type != *prev_dst_type &&
				   (dst_reg_type == PTR_TO_CTX ||
				    *prev_dst_type == PTR_TO_CTX)) {
				verbose(env, "same insn cannot be used with different pointers\n");
				return -EINVAL;
			}

		} else if (class == BPF_ST) {
			if (BPF_MODE(insn->code) != BPF_MEM ||
			    insn->src_reg != BPF_REG_0) {
				verbose(env, "BPF_ST uses reserved fields\n");
				return -EINVAL;
			}
			/* check src operand */
			err = check_reg_arg(env, insn->dst_reg, SRC_OP);
			if (err)
				return err;

			if (is_ctx_reg(env, insn->dst_reg)) {
				verbose(env, "BPF_ST stores into R%d context is not allowed\n",
					insn->dst_reg);
				return -EACCES;
			}

			/* check that memory (dst_reg + off) is writeable */
			err = check_mem_access(env, insn_idx, insn->dst_reg, insn->off,
					       BPF_SIZE(insn->code), BPF_WRITE,
					       -1, false);
			if (err)
				return err;

		} else if (class == BPF_JMP) {
			u8 opcode = BPF_OP(insn->code);

			if (opcode == BPF_CALL) {
				if (BPF_SRC(insn->code) != BPF_K ||
				    insn->off != 0 ||
				    (insn->src_reg != BPF_REG_0 &&
				     insn->src_reg != BPF_PSEUDO_CALL) ||
				    insn->dst_reg != BPF_REG_0) {
					verbose(env, "BPF_CALL uses reserved fields\n");
					return -EINVAL;
				}

				if (insn->src_reg == BPF_PSEUDO_CALL)
					err = check_func_call(env, insn, &insn_idx);
				else
					err = check_helper_call(env, insn->imm, insn_idx);
				if (err)
					return err;

			} else if (opcode == BPF_JA) {
				if (BPF_SRC(insn->code) != BPF_K ||
				    insn->imm != 0 ||
				    insn->src_reg != BPF_REG_0 ||
				    insn->dst_reg != BPF_REG_0) {
					verbose(env, "BPF_JA uses reserved fields\n");
					return -EINVAL;
				}

				insn_idx += insn->off + 1;
				continue;

			} else if (opcode == BPF_EXIT) {
				if (BPF_SRC(insn->code) != BPF_K ||
				    insn->imm != 0 ||
				    insn->src_reg != BPF_REG_0 ||
				    insn->dst_reg != BPF_REG_0) {
					verbose(env, "BPF_EXIT uses reserved fields\n");
					return -EINVAL;
				}

				if (state->curframe) {
					/* exit from nested function */
					prev_insn_idx = insn_idx;
					err = prepare_func_exit(env, &insn_idx);
					if (err)
						return err;
					do_print_state = true;
					continue;
				}

				/* eBPF calling convetion is such that R0 is used
				 * to return the value from eBPF program.
				 * Make sure that it's readable at this time
				 * of bpf_exit, which means that program wrote
				 * something into it earlier
				 */
				err = check_reg_arg(env, BPF_REG_0, SRC_OP);
				if (err)
					return err;

				if (is_pointer_value(env, BPF_REG_0)) {
					verbose(env, "R0 leaks addr as return value\n");
					return -EACCES;
				}

				err = check_return_code(env);
				if (err)
					return err;
process_bpf_exit:
				err = pop_stack(env, &prev_insn_idx, &insn_idx);
				if (err < 0) {
					if (err != -ENOENT)
						return err;
					break;
				} else {
					do_print_state = true;
					continue;
				}
			} else {
				err = check_cond_jmp_op(env, insn, &insn_idx);
				if (err)
					return err;
			}
		} else if (class == BPF_LD) {
			u8 mode = BPF_MODE(insn->code);

			if (mode == BPF_ABS || mode == BPF_IND) {
				err = check_ld_abs(env, insn);
				if (err)
					return err;

			} else if (mode == BPF_IMM) {
				err = check_ld_imm(env, insn);
				if (err)
					return err;

				insn_idx++;
				env->insn_aux_data[insn_idx].seen = true;
			} else {
				verbose(env, "invalid BPF_LD mode\n");
				return -EINVAL;
			}
		} else {
			verbose(env, "unknown insn class %d\n", class);
			return -EINVAL;
		}

		insn_idx++;
	}

	verbose(env, "processed %d insns (limit %d), stack depth ",
		insn_processed, BPF_COMPLEXITY_LIMIT_INSNS);
	for (i = 0; i < env->subprog_cnt; i++) {
		u32 depth = env->subprog_info[i].stack_depth;

		verbose(env, "%d", depth);
		if (i + 1 < env->subprog_cnt)
			verbose(env, "+");
	}
	verbose(env, "\n");
	env->prog->aux->stack_depth = env->subprog_info[0].stack_depth;
	return 0;
}