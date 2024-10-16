static void uvesafb_vbe_getstatesize(struct uvesafb_ktask *task,
				     struct uvesafb_par *par)
{
	int err;

	uvesafb_reset(task);

	/*
	 * Get the VBE state buffer size. We want all available
	 * hardware state data (CL = 0x0f).
	 */
	task->t.regs.eax = 0x4f04;
	task->t.regs.ecx = 0x000f;
	task->t.regs.edx = 0x0000;
	task->t.flags = 0;

	err = uvesafb_exec(task);

	if (err || (task->t.regs.eax & 0xffff) != 0x004f) {
		pr_warn("VBE state buffer size cannot be determined (eax=0x%x, err=%d)\n",
			task->t.regs.eax, err);
		par->vbe_state_size = 0;
		return;
	}

	par->vbe_state_size = 64 * (task->t.regs.ebx & 0xffff);
}