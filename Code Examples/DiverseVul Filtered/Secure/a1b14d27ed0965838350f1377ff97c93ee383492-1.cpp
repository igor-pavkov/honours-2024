static int check_cond_jmp_op(struct verifier_env *env,
			     struct bpf_insn *insn, int *insn_idx)
{
	struct reg_state *regs = env->cur_state.regs;
	struct verifier_state *other_branch;
	u8 opcode = BPF_OP(insn->code);
	int err;

	if (opcode > BPF_EXIT) {
		verbose("invalid BPF_JMP opcode %x\n", opcode);
		return -EINVAL;
	}

	if (BPF_SRC(insn->code) == BPF_X) {
		if (insn->imm != 0) {
			verbose("BPF_JMP uses reserved fields\n");
			return -EINVAL;
		}

		/* check src1 operand */
		err = check_reg_arg(regs, insn->src_reg, SRC_OP);
		if (err)
			return err;

		if (is_pointer_value(env, insn->src_reg)) {
			verbose("R%d pointer comparison prohibited\n",
				insn->src_reg);
			return -EACCES;
		}
	} else {
		if (insn->src_reg != BPF_REG_0) {
			verbose("BPF_JMP uses reserved fields\n");
			return -EINVAL;
		}
	}

	/* check src2 operand */
	err = check_reg_arg(regs, insn->dst_reg, SRC_OP);
	if (err)
		return err;

	/* detect if R == 0 where R was initialized to zero earlier */
	if (BPF_SRC(insn->code) == BPF_K &&
	    (opcode == BPF_JEQ || opcode == BPF_JNE) &&
	    regs[insn->dst_reg].type == CONST_IMM &&
	    regs[insn->dst_reg].imm == insn->imm) {
		if (opcode == BPF_JEQ) {
			/* if (imm == imm) goto pc+off;
			 * only follow the goto, ignore fall-through
			 */
			*insn_idx += insn->off;
			return 0;
		} else {
			/* if (imm != imm) goto pc+off;
			 * only follow fall-through branch, since
			 * that's where the program will go
			 */
			return 0;
		}
	}

	other_branch = push_stack(env, *insn_idx + insn->off + 1, *insn_idx);
	if (!other_branch)
		return -EFAULT;

	/* detect if R == 0 where R is returned value from bpf_map_lookup_elem() */
	if (BPF_SRC(insn->code) == BPF_K &&
	    insn->imm == 0 && (opcode == BPF_JEQ ||
			       opcode == BPF_JNE) &&
	    regs[insn->dst_reg].type == PTR_TO_MAP_VALUE_OR_NULL) {
		if (opcode == BPF_JEQ) {
			/* next fallthrough insn can access memory via
			 * this register
			 */
			regs[insn->dst_reg].type = PTR_TO_MAP_VALUE;
			/* branch targer cannot access it, since reg == 0 */
			other_branch->regs[insn->dst_reg].type = CONST_IMM;
			other_branch->regs[insn->dst_reg].imm = 0;
		} else {
			other_branch->regs[insn->dst_reg].type = PTR_TO_MAP_VALUE;
			regs[insn->dst_reg].type = CONST_IMM;
			regs[insn->dst_reg].imm = 0;
		}
	} else if (is_pointer_value(env, insn->dst_reg)) {
		verbose("R%d pointer comparison prohibited\n", insn->dst_reg);
		return -EACCES;
	} else if (BPF_SRC(insn->code) == BPF_K &&
		   (opcode == BPF_JEQ || opcode == BPF_JNE)) {

		if (opcode == BPF_JEQ) {
			/* detect if (R == imm) goto
			 * and in the target state recognize that R = imm
			 */
			other_branch->regs[insn->dst_reg].type = CONST_IMM;
			other_branch->regs[insn->dst_reg].imm = insn->imm;
		} else {
			/* detect if (R != imm) goto
			 * and in the fall-through state recognize that R = imm
			 */
			regs[insn->dst_reg].type = CONST_IMM;
			regs[insn->dst_reg].imm = insn->imm;
		}
	}
	if (log_level)
		print_verifier_state(env);
	return 0;
}