static int uvesafb_vbe_getmodes(struct uvesafb_ktask *task,
				struct uvesafb_par *par)
{
	int off = 0, err;
	u16 *mode;

	par->vbe_modes_cnt = 0;

	/* Count available modes. */
	mode = (u16 *) (((u8 *)&par->vbe_ib) + par->vbe_ib.mode_list_ptr);
	while (*mode != 0xffff) {
		par->vbe_modes_cnt++;
		mode++;
	}

	par->vbe_modes = kzalloc(sizeof(struct vbe_mode_ib) *
				par->vbe_modes_cnt, GFP_KERNEL);
	if (!par->vbe_modes)
		return -ENOMEM;

	/* Get info about all available modes. */
	mode = (u16 *) (((u8 *)&par->vbe_ib) + par->vbe_ib.mode_list_ptr);
	while (*mode != 0xffff) {
		struct vbe_mode_ib *mib;

		uvesafb_reset(task);
		task->t.regs.eax = 0x4f01;
		task->t.regs.ecx = (u32) *mode;
		task->t.flags = TF_BUF_RET | TF_BUF_ESDI;
		task->t.buf_len = sizeof(struct vbe_mode_ib);
		task->buf = par->vbe_modes + off;

		err = uvesafb_exec(task);
		if (err || (task->t.regs.eax & 0xffff) != 0x004f) {
			pr_warn("Getting mode info block for mode 0x%x failed (eax=0x%x, err=%d)\n",
				*mode, (u32)task->t.regs.eax, err);
			mode++;
			par->vbe_modes_cnt--;
			continue;
		}

		mib = task->buf;
		mib->mode_id = *mode;

		/*
		 * We only want modes that are supported with the current
		 * hardware configuration, color, graphics and that have
		 * support for the LFB.
		 */
		if ((mib->mode_attr & VBE_MODE_MASK) == VBE_MODE_MASK &&
				 mib->bits_per_pixel >= 8)
			off++;
		else
			par->vbe_modes_cnt--;

		mode++;
		mib->depth = mib->red_len + mib->green_len + mib->blue_len;

		/*
		 * Handle 8bpp modes and modes with broken color component
		 * lengths.
		 */
		if (mib->depth == 0 || (mib->depth == 24 &&
					mib->bits_per_pixel == 32))
			mib->depth = mib->bits_per_pixel;
	}

	if (par->vbe_modes_cnt > 0)
		return 0;
	else
		return -EINVAL;
}