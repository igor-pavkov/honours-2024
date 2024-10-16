static int uvesafb_vbe_getpmi(struct uvesafb_ktask *task,
			      struct uvesafb_par *par)
{
	int i, err;

	uvesafb_reset(task);
	task->t.regs.eax = 0x4f0a;
	task->t.regs.ebx = 0x0;
	err = uvesafb_exec(task);

	if ((task->t.regs.eax & 0xffff) != 0x4f || task->t.regs.es < 0xc000) {
		par->pmi_setpal = par->ypan = 0;
	} else {
		par->pmi_base = (u16 *)phys_to_virt(((u32)task->t.regs.es << 4)
						+ task->t.regs.edi);
		par->pmi_start = (u8 *)par->pmi_base + par->pmi_base[1];
		par->pmi_pal = (u8 *)par->pmi_base + par->pmi_base[2];
		pr_info("protected mode interface info at %04x:%04x\n",
			(u16)task->t.regs.es, (u16)task->t.regs.edi);
		pr_info("pmi: set display start = %p, set palette = %p\n",
			par->pmi_start, par->pmi_pal);

		if (par->pmi_base[3]) {
			pr_info("pmi: ports =");
			for (i = par->pmi_base[3]/2;
					par->pmi_base[i] != 0xffff; i++)
				pr_cont(" %x", par->pmi_base[i]);
			pr_cont("\n");

			if (par->pmi_base[i] != 0xffff) {
				pr_info("can't handle memory requests, pmi disabled\n");
				par->ypan = par->pmi_setpal = 0;
			}
		}
	}
	return 0;
}