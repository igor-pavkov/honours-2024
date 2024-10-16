static int uvesafb_blank(int blank, struct fb_info *info)
{
	struct uvesafb_ktask *task;
	int err = 1;
#ifdef CONFIG_X86
	struct uvesafb_par *par = info->par;

	if (par->vbe_ib.capabilities & VBE_CAP_VGACOMPAT) {
		int loop = 10000;
		u8 seq = 0, crtc17 = 0;

		if (blank == FB_BLANK_POWERDOWN) {
			seq = 0x20;
			crtc17 = 0x00;
			err = 0;
		} else {
			seq = 0x00;
			crtc17 = 0x80;
			err = (blank == FB_BLANK_UNBLANK) ? 0 : -EINVAL;
		}

		vga_wseq(NULL, 0x00, 0x01);
		seq |= vga_rseq(NULL, 0x01) & ~0x20;
		vga_wseq(NULL, 0x00, seq);

		crtc17 |= vga_rcrt(NULL, 0x17) & ~0x80;
		while (loop--);
		vga_wcrt(NULL, 0x17, crtc17);
		vga_wseq(NULL, 0x00, 0x03);
	} else
#endif /* CONFIG_X86 */
	{
		task = uvesafb_prep();
		if (!task)
			return -ENOMEM;

		task->t.regs.eax = 0x4f10;
		switch (blank) {
		case FB_BLANK_UNBLANK:
			task->t.regs.ebx = 0x0001;
			break;
		case FB_BLANK_NORMAL:
			task->t.regs.ebx = 0x0101;	/* standby */
			break;
		case FB_BLANK_POWERDOWN:
			task->t.regs.ebx = 0x0401;	/* powerdown */
			break;
		default:
			goto out;
		}

		err = uvesafb_exec(task);
		if (err || (task->t.regs.eax & 0xffff) != 0x004f)
			err = 1;
out:		uvesafb_free(task);
	}
	return err;
}