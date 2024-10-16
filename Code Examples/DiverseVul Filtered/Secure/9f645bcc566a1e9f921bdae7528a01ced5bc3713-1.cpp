static int uvesafb_vbe_init_mode(struct fb_info *info)
{
	struct list_head *pos;
	struct fb_modelist *modelist;
	struct fb_videomode *mode;
	struct uvesafb_par *par = info->par;
	int i, modeid;

	/* Has the user requested a specific VESA mode? */
	if (vbemode) {
		for (i = 0; i < par->vbe_modes_cnt; i++) {
			if (par->vbe_modes[i].mode_id == vbemode) {
				modeid = i;
				uvesafb_setup_var(&info->var, info,
						&par->vbe_modes[modeid]);
				fb_get_mode(FB_VSYNCTIMINGS | FB_IGNOREMON, 60,
						&info->var, info);
				/*
				 * With pixclock set to 0, the default BIOS
				 * timings will be used in set_par().
				 */
				info->var.pixclock = 0;
				goto gotmode;
			}
		}
		pr_info("requested VBE mode 0x%x is unavailable\n", vbemode);
		vbemode = 0;
	}

	/* Count the modes in the modelist */
	i = 0;
	list_for_each(pos, &info->modelist)
		i++;

	/*
	 * Convert the modelist into a modedb so that we can use it with
	 * fb_find_mode().
	 */
	mode = kzalloc(i * sizeof(*mode), GFP_KERNEL);
	if (mode) {
		i = 0;
		list_for_each(pos, &info->modelist) {
			modelist = list_entry(pos, struct fb_modelist, list);
			mode[i] = modelist->mode;
			i++;
		}

		if (!mode_option)
			mode_option = UVESAFB_DEFAULT_MODE;

		i = fb_find_mode(&info->var, info, mode_option, mode, i,
			NULL, 8);

		kfree(mode);
	}

	/* fb_find_mode() failed */
	if (i == 0) {
		info->var.xres = 640;
		info->var.yres = 480;
		mode = (struct fb_videomode *)
				fb_find_best_mode(&info->var, &info->modelist);

		if (mode) {
			fb_videomode_to_var(&info->var, mode);
		} else {
			modeid = par->vbe_modes[0].mode_id;
			uvesafb_setup_var(&info->var, info,
					&par->vbe_modes[modeid]);
			fb_get_mode(FB_VSYNCTIMINGS | FB_IGNOREMON, 60,
					&info->var, info);

			goto gotmode;
		}
	}

	/* Look for a matching VBE mode. */
	modeid = uvesafb_vbe_find_mode(par, info->var.xres, info->var.yres,
			info->var.bits_per_pixel, UVESAFB_EXACT_RES);

	if (modeid == -1)
		return -EINVAL;

	uvesafb_setup_var(&info->var, info, &par->vbe_modes[modeid]);

gotmode:
	/*
	 * If we are not VBE3.0+ compliant, we're done -- the BIOS will
	 * ignore our timings anyway.
	 */
	if (par->vbe_ib.vbe_version < 0x0300 || par->nocrtc)
		fb_get_mode(FB_VSYNCTIMINGS | FB_IGNOREMON, 60,
					&info->var, info);

	return modeid;
}