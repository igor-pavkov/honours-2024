static int smtc_check_var(struct fb_var_screeninfo *var, struct fb_info *info)
{
	/* sanity checks */
	if (var->xres_virtual < var->xres)
		var->xres_virtual = var->xres;

	if (var->yres_virtual < var->yres)
		var->yres_virtual = var->yres;

	/* set valid default bpp */
	if ((var->bits_per_pixel != 8)  && (var->bits_per_pixel != 16) &&
	    (var->bits_per_pixel != 24) && (var->bits_per_pixel != 32))
		var->bits_per_pixel = 16;

	return 0;
}