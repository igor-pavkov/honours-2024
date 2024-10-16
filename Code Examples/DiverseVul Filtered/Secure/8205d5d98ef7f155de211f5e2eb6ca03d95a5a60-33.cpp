static void show_instructions(struct pt_regs *regs)
{
	int i;
	unsigned long pc = regs->nip - (NR_INSN_TO_PRINT * 3 / 4 * sizeof(int));

	printk("Instruction dump:");

	for (i = 0; i < NR_INSN_TO_PRINT; i++) {
		int instr;

		if (!(i % 8))
			pr_cont("\n");

#if !defined(CONFIG_BOOKE)
		/* If executing with the IMMU off, adjust pc rather
		 * than print XXXXXXXX.
		 */
		if (!(regs->msr & MSR_IR))
			pc = (unsigned long)phys_to_virt(pc);
#endif

		if (!__kernel_text_address(pc) ||
		    probe_kernel_address((const void *)pc, instr)) {
			pr_cont("XXXXXXXX ");
		} else {
			if (regs->nip == pc)
				pr_cont("<%08x> ", instr);
			else
				pr_cont("%08x ", instr);
		}

		pc += sizeof(int);
	}

	pr_cont("\n");
}