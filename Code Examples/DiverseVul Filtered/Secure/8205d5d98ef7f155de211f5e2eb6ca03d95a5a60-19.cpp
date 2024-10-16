void show_user_instructions(struct pt_regs *regs)
{
	unsigned long pc;
	int n = NR_INSN_TO_PRINT;
	struct seq_buf s;
	char buf[96]; /* enough for 8 times 9 + 2 chars */

	pc = regs->nip - (NR_INSN_TO_PRINT * 3 / 4 * sizeof(int));

	/*
	 * Make sure the NIP points at userspace, not kernel text/data or
	 * elsewhere.
	 */
	if (!__access_ok(pc, NR_INSN_TO_PRINT * sizeof(int), USER_DS)) {
		pr_info("%s[%d]: Bad NIP, not dumping instructions.\n",
			current->comm, current->pid);
		return;
	}

	seq_buf_init(&s, buf, sizeof(buf));

	while (n) {
		int i;

		seq_buf_clear(&s);

		for (i = 0; i < 8 && n; i++, n--, pc += sizeof(int)) {
			int instr;

			if (probe_kernel_address((const void *)pc, instr)) {
				seq_buf_printf(&s, "XXXXXXXX ");
				continue;
			}
			seq_buf_printf(&s, regs->nip == pc ? "<%08x> " : "%08x ", instr);
		}

		if (!seq_buf_has_overflowed(&s))
			pr_info("%s[%d]: code: %s\n", current->comm,
				current->pid, s.buffer);
	}
}