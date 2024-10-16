static int uvesafb_is_valid_mode(struct fb_videomode *mode,
				 struct fb_info *info)
{
	if (info->monspecs.gtf) {
		fb_videomode_to_var(&info->var, mode);
		if (fb_validate_mode(&info->var, info))
			return 0;
	}

	if (uvesafb_vbe_find_mode(info->par, mode->xres, mode->yres, 8,
				UVESAFB_EXACT_RES) == -1)
		return 0;

	return 1;
}