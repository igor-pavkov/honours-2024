static void uvesafb_vbe_getmonspecs(struct uvesafb_ktask *task,
				    struct fb_info *info)
{
	struct uvesafb_par *par = info->par;
	int i;

	memset(&info->monspecs, 0, sizeof(info->monspecs));

	/*
	 * If we don't get all necessary data from the EDID block,
	 * mark it as incompatible with the GTF and set nocrtc so
	 * that we always use the default BIOS refresh rate.
	 */
	if (uvesafb_vbe_getedid(task, info)) {
		info->monspecs.gtf = 0;
		par->nocrtc = 1;
	}

	/* Kernel command line overrides. */
	if (maxclk)
		info->monspecs.dclkmax = maxclk * 1000000;
	if (maxvf)
		info->monspecs.vfmax = maxvf;
	if (maxhf)
		info->monspecs.hfmax = maxhf * 1000;

	/*
	 * In case DDC transfers are not supported, the user can provide
	 * monitor limits manually. Lower limits are set to "safe" values.
	 */
	if (info->monspecs.gtf == 0 && maxclk && maxvf && maxhf) {
		info->monspecs.dclkmin = 0;
		info->monspecs.vfmin = 60;
		info->monspecs.hfmin = 29000;
		info->monspecs.gtf = 1;
		par->nocrtc = 0;
	}

	if (info->monspecs.gtf)
		pr_info("monitor limits: vf = %d Hz, hf = %d kHz, clk = %d MHz\n",
			info->monspecs.vfmax,
			(int)(info->monspecs.hfmax / 1000),
			(int)(info->monspecs.dclkmax / 1000000));
	else
		pr_info("no monitor limits have been set, default refresh rate will be used\n");

	/* Add VBE modes to the modelist. */
	for (i = 0; i < par->vbe_modes_cnt; i++) {
		struct fb_var_screeninfo var;
		struct vbe_mode_ib *mode;
		struct fb_videomode vmode;

		mode = &par->vbe_modes[i];
		memset(&var, 0, sizeof(var));

		var.xres = mode->x_res;
		var.yres = mode->y_res;

		fb_get_mode(FB_VSYNCTIMINGS | FB_IGNOREMON, 60, &var, info);
		fb_var_to_videomode(&vmode, &var);
		fb_add_videomode(&vmode, &info->modelist);
	}

	/* Add valid VESA modes to our modelist. */
	for (i = 0; i < VESA_MODEDB_SIZE; i++) {
		if (uvesafb_is_valid_mode((struct fb_videomode *)
						&vesa_modes[i], info))
			fb_add_videomode(&vesa_modes[i], &info->modelist);
	}

	for (i = 0; i < info->monspecs.modedb_len; i++) {
		if (uvesafb_is_valid_mode(&info->monspecs.modedb[i], info))
			fb_add_videomode(&info->monspecs.modedb[i],
					&info->modelist);
	}

	return;
}