static void fn_show_ptregs(struct vc_data *vc)
{
	struct pt_regs *regs = get_irq_regs();

	if (regs)
		show_regs(regs);
}