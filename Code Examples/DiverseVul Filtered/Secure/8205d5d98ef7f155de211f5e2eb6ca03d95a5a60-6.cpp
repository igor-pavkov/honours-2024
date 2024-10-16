void show_regs(struct pt_regs * regs)
{
	int i, trap;

	show_regs_print_info(KERN_DEFAULT);

	printk("NIP:  "REG" LR: "REG" CTR: "REG"\n",
	       regs->nip, regs->link, regs->ctr);
	printk("REGS: %px TRAP: %04lx   %s  (%s)\n",
	       regs, regs->trap, print_tainted(), init_utsname()->release);
	printk("MSR:  "REG" ", regs->msr);
	print_msr_bits(regs->msr);
	pr_cont("  CR: %08lx  XER: %08lx\n", regs->ccr, regs->xer);
	trap = TRAP(regs);
	if ((TRAP(regs) != 0xc00) && cpu_has_feature(CPU_FTR_CFAR))
		pr_cont("CFAR: "REG" ", regs->orig_gpr3);
	if (trap == 0x200 || trap == 0x300 || trap == 0x600)
#if defined(CONFIG_4xx) || defined(CONFIG_BOOKE)
		pr_cont("DEAR: "REG" ESR: "REG" ", regs->dar, regs->dsisr);
#else
		pr_cont("DAR: "REG" DSISR: %08lx ", regs->dar, regs->dsisr);
#endif
#ifdef CONFIG_PPC64
	pr_cont("IRQMASK: %lx ", regs->softe);
#endif
#ifdef CONFIG_PPC_TRANSACTIONAL_MEM
	if (MSR_TM_ACTIVE(regs->msr))
		pr_cont("\nPACATMSCRATCH: %016llx ", get_paca()->tm_scratch);
#endif

	for (i = 0;  i < 32;  i++) {
		if ((i % REGS_PER_LINE) == 0)
			pr_cont("\nGPR%02d: ", i);
		pr_cont(REG " ", regs->gpr[i]);
		if (i == LAST_VOLATILE && !FULL_REGS(regs))
			break;
	}
	pr_cont("\n");
#ifdef CONFIG_KALLSYMS
	/*
	 * Lookup NIP late so we have the best change of getting the
	 * above info out without failing
	 */
	printk("NIP ["REG"] %pS\n", regs->nip, (void *)regs->nip);
	printk("LR ["REG"] %pS\n", regs->link, (void *)regs->link);
#endif
	show_stack(current, (unsigned long *) regs->gpr[1]);
	if (!user_mode(regs))
		show_instructions(regs);
}