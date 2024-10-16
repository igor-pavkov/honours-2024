static void uvesafb_check_limits(struct fb_var_screeninfo *var,
		struct fb_info *info)
{
	const struct fb_videomode *mode;
	struct uvesafb_par *par = info->par;

	/*
	 * If pixclock is set to 0, then we're using default BIOS timings
	 * and thus don't have to perform any checks here.
	 */
	if (!var->pixclock)
		return;

	if (par->vbe_ib.vbe_version < 0x0300) {
		fb_get_mode(FB_VSYNCTIMINGS | FB_IGNOREMON, 60, var, info);
		return;
	}

	if (!fb_validate_mode(var, info))
		return;

	mode = fb_find_best_mode(var, &info->modelist);
	if (mode) {
		if (mode->xres == var->xres && mode->yres == var->yres &&
		    !(mode->vmode & (FB_VMODE_INTERLACED | FB_VMODE_DOUBLE))) {
			fb_videomode_to_var(var, mode);
			return;
		}
	}

	if (info->monspecs.gtf && !fb_get_mode(FB_MAXTIMINGS, 0, var, info))
		return;
	/* Use default refresh rate */
	var->pixclock = 0;
}