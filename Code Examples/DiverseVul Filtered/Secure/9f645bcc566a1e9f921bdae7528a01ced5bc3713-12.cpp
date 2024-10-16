static int uvesafb_vbe_getedid(struct uvesafb_ktask *task, struct fb_info *info)
{
	struct uvesafb_par *par = info->par;
	int err = 0;

	if (noedid || par->vbe_ib.vbe_version < 0x0300)
		return -EINVAL;

	task->t.regs.eax = 0x4f15;
	task->t.regs.ebx = 0;
	task->t.regs.ecx = 0;
	task->t.buf_len = 0;
	task->t.flags = 0;

	err = uvesafb_exec(task);

	if ((task->t.regs.eax & 0xffff) != 0x004f || err)
		return -EINVAL;

	if ((task->t.regs.ebx & 0x3) == 3) {
		pr_info("VBIOS/hardware supports both DDC1 and DDC2 transfers\n");
	} else if ((task->t.regs.ebx & 0x3) == 2) {
		pr_info("VBIOS/hardware supports DDC2 transfers\n");
	} else if ((task->t.regs.ebx & 0x3) == 1) {
		pr_info("VBIOS/hardware supports DDC1 transfers\n");
	} else {
		pr_info("VBIOS/hardware doesn't support DDC transfers\n");
		return -EINVAL;
	}

	task->t.regs.eax = 0x4f15;
	task->t.regs.ebx = 1;
	task->t.regs.ecx = task->t.regs.edx = 0;
	task->t.flags = TF_BUF_RET | TF_BUF_ESDI;
	task->t.buf_len = EDID_LENGTH;
	task->buf = kzalloc(EDID_LENGTH, GFP_KERNEL);
	if (!task->buf)
		return -ENOMEM;

	err = uvesafb_exec(task);

	if ((task->t.regs.eax & 0xffff) == 0x004f && !err) {
		fb_edid_to_monspecs(task->buf, &info->monspecs);

		if (info->monspecs.vfmax && info->monspecs.hfmax) {
			/*
			 * If the maximum pixel clock wasn't specified in
			 * the EDID block, set it to 300 MHz.
			 */
			if (info->monspecs.dclkmax == 0)
				info->monspecs.dclkmax = 300 * 1000000;
			info->monspecs.gtf = 1;
		}
	} else {
		err = -EINVAL;
	}

	kfree(task->buf);
	return err;
}