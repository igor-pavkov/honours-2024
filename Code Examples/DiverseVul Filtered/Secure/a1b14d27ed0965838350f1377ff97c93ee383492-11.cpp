static int convert_ctx_accesses(struct verifier_env *env)
{
	struct bpf_insn *insn = env->prog->insnsi;
	int insn_cnt = env->prog->len;
	struct bpf_insn insn_buf[16];
	struct bpf_prog *new_prog;
	u32 cnt;
	int i;
	enum bpf_access_type type;

	if (!env->prog->aux->ops->convert_ctx_access)
		return 0;

	for (i = 0; i < insn_cnt; i++, insn++) {
		if (insn->code == (BPF_LDX | BPF_MEM | BPF_W))
			type = BPF_READ;
		else if (insn->code == (BPF_STX | BPF_MEM | BPF_W))
			type = BPF_WRITE;
		else
			continue;

		if (insn->imm != PTR_TO_CTX) {
			/* clear internal mark */
			insn->imm = 0;
			continue;
		}

		cnt = env->prog->aux->ops->
			convert_ctx_access(type, insn->dst_reg, insn->src_reg,
					   insn->off, insn_buf, env->prog);
		if (cnt == 0 || cnt >= ARRAY_SIZE(insn_buf)) {
			verbose("bpf verifier is misconfigured\n");
			return -EINVAL;
		}

		if (cnt == 1) {
			memcpy(insn, insn_buf, sizeof(*insn));
			continue;
		}

		/* several new insns need to be inserted. Make room for them */
		insn_cnt += cnt - 1;
		new_prog = bpf_prog_realloc(env->prog,
					    bpf_prog_size(insn_cnt),
					    GFP_USER);
		if (!new_prog)
			return -ENOMEM;

		new_prog->len = insn_cnt;

		memmove(new_prog->insnsi + i + cnt, new_prog->insns + i + 1,
			sizeof(*insn) * (insn_cnt - i - cnt));

		/* copy substitute insns in place of load instruction */
		memcpy(new_prog->insnsi + i, insn_buf, sizeof(*insn) * cnt);

		/* adjust branches in the whole program */
		adjust_branches(new_prog, i, cnt - 1);

		/* keep walking new program and skip insns we just inserted */
		env->prog = new_prog;
		insn = new_prog->insnsi + i + cnt - 1;
		i += cnt - 1;
	}

	return 0;
}