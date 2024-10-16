static void adjust_branches(struct bpf_prog *prog, int pos, int delta)
{
	struct bpf_insn *insn = prog->insnsi;
	int insn_cnt = prog->len;
	int i;

	for (i = 0; i < insn_cnt; i++, insn++) {
		if (BPF_CLASS(insn->code) != BPF_JMP ||
		    BPF_OP(insn->code) == BPF_CALL ||
		    BPF_OP(insn->code) == BPF_EXIT)
			continue;

		/* adjust offset of jmps if necessary */
		if (i < pos && i + insn->off + 1 > pos)
			insn->off += delta;
		else if (i > pos + delta && i + insn->off + 1 <= pos + delta)
			insn->off -= delta;
	}
}