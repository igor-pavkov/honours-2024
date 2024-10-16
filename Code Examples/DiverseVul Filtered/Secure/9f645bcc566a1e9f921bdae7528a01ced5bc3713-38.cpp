static int uvesafb_set_par(struct fb_info *info)
{
	struct uvesafb_par *par = info->par;
	struct uvesafb_ktask *task = NULL;
	struct vbe_crtc_ib *crtc = NULL;
	struct vbe_mode_ib *mode = NULL;
	int i, err = 0, depth = info->var.bits_per_pixel;

	if (depth > 8 && depth != 32)
		depth = info->var.red.length + info->var.green.length +
			info->var.blue.length;

	i = uvesafb_vbe_find_mode(par, info->var.xres, info->var.yres, depth,
				 UVESAFB_EXACT_RES | UVESAFB_EXACT_DEPTH);
	if (i >= 0)
		mode = &par->vbe_modes[i];
	else
		return -EINVAL;

	task = uvesafb_prep();
	if (!task)
		return -ENOMEM;
setmode:
	task->t.regs.eax = 0x4f02;
	task->t.regs.ebx = mode->mode_id | 0x4000;	/* use LFB */

	if (par->vbe_ib.vbe_version >= 0x0300 && !par->nocrtc &&
	    info->var.pixclock != 0) {
		task->t.regs.ebx |= 0x0800;		/* use CRTC data */
		task->t.flags = TF_BUF_ESDI;
		crtc = kzalloc(sizeof(struct vbe_crtc_ib), GFP_KERNEL);
		if (!crtc) {
			err = -ENOMEM;
			goto out;
		}
		crtc->horiz_start = info->var.xres + info->var.right_margin;
		crtc->horiz_end	  = crtc->horiz_start + info->var.hsync_len;
		crtc->horiz_total = crtc->horiz_end + info->var.left_margin;

		crtc->vert_start  = info->var.yres + info->var.lower_margin;
		crtc->vert_end    = crtc->vert_start + info->var.vsync_len;
		crtc->vert_total  = crtc->vert_end + info->var.upper_margin;

		crtc->pixel_clock = PICOS2KHZ(info->var.pixclock) * 1000;
		crtc->refresh_rate = (u16)(100 * (crtc->pixel_clock /
				(crtc->vert_total * crtc->horiz_total)));

		if (info->var.vmode & FB_VMODE_DOUBLE)
			crtc->flags |= 0x1;
		if (info->var.vmode & FB_VMODE_INTERLACED)
			crtc->flags |= 0x2;
		if (!(info->var.sync & FB_SYNC_HOR_HIGH_ACT))
			crtc->flags |= 0x4;
		if (!(info->var.sync & FB_SYNC_VERT_HIGH_ACT))
			crtc->flags |= 0x8;
		memcpy(&par->crtc, crtc, sizeof(*crtc));
	} else {
		memset(&par->crtc, 0, sizeof(*crtc));
	}

	task->t.buf_len = sizeof(struct vbe_crtc_ib);
	task->buf = &par->crtc;

	err = uvesafb_exec(task);
	if (err || (task->t.regs.eax & 0xffff) != 0x004f) {
		/*
		 * The mode switch might have failed because we tried to
		 * use our own timings.  Try again with the default timings.
		 */
		if (crtc != NULL) {
			pr_warn("mode switch failed (eax=0x%x, err=%d) - trying again with default timings\n",
				task->t.regs.eax, err);
			uvesafb_reset(task);
			kfree(crtc);
			crtc = NULL;
			info->var.pixclock = 0;
			goto setmode;
		} else {
			pr_err("mode switch failed (eax=0x%x, err=%d)\n",
			       task->t.regs.eax, err);
			err = -EINVAL;
			goto out;
		}
	}
	par->mode_idx = i;

	/* For 8bpp modes, always try to set the DAC to 8 bits. */
	if (par->vbe_ib.capabilities & VBE_CAP_CAN_SWITCH_DAC &&
	    mode->bits_per_pixel <= 8) {
		uvesafb_reset(task);
		task->t.regs.eax = 0x4f08;
		task->t.regs.ebx = 0x0800;

		err = uvesafb_exec(task);
		if (err || (task->t.regs.eax & 0xffff) != 0x004f ||
		    ((task->t.regs.ebx & 0xff00) >> 8) != 8) {
			dac_width = 6;
		} else {
			dac_width = 8;
		}
	}

	info->fix.visual = (info->var.bits_per_pixel == 8) ?
				FB_VISUAL_PSEUDOCOLOR : FB_VISUAL_TRUECOLOR;
	info->fix.line_length = mode->bytes_per_scan_line;

out:
	kfree(crtc);
	uvesafb_free(task);

	return err;
}