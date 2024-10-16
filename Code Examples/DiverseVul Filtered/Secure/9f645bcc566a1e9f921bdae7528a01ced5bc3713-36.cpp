static int uvesafb_check_var(struct fb_var_screeninfo *var,
		struct fb_info *info)
{
	struct uvesafb_par *par = info->par;
	struct vbe_mode_ib *mode = NULL;
	int match = -1;
	int depth = var->red.length + var->green.length + var->blue.length;

	/*
	 * Various apps will use bits_per_pixel to set the color depth,
	 * which is theoretically incorrect, but which we'll try to handle
	 * here.
	 */
	if (depth == 0 || abs(depth - var->bits_per_pixel) >= 8)
		depth = var->bits_per_pixel;

	match = uvesafb_vbe_find_mode(par, var->xres, var->yres, depth,
						UVESAFB_EXACT_RES);
	if (match == -1)
		return -EINVAL;

	mode = &par->vbe_modes[match];
	uvesafb_setup_var(var, info, mode);

	/*
	 * Check whether we have remapped enough memory for this mode.
	 * We might be called at an early stage, when we haven't remapped
	 * any memory yet, in which case we simply skip the check.
	 */
	if (var->yres * mode->bytes_per_scan_line > info->fix.smem_len
						&& info->fix.smem_len)
		return -EINVAL;

	if ((var->vmode & FB_VMODE_DOUBLE) &&
				!(par->vbe_modes[match].mode_attr & 0x100))
		var->vmode &= ~FB_VMODE_DOUBLE;

	if ((var->vmode & FB_VMODE_INTERLACED) &&
				!(par->vbe_modes[match].mode_attr & 0x200))
		var->vmode &= ~FB_VMODE_INTERLACED;

	uvesafb_check_limits(var, info);

	var->xres_virtual = var->xres;
	var->yres_virtual = (par->ypan) ?
				info->fix.smem_len / mode->bytes_per_scan_line :
				var->yres;
	return 0;
}