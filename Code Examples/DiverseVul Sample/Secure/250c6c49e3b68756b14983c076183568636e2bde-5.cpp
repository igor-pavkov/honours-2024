void sbusfb_fill_var(struct fb_var_screeninfo *var, struct device_node *dp,
		     int bpp)
{
	memset(var, 0, sizeof(*var));

	var->xres = of_getintprop_default(dp, "width", 1152);
	var->yres = of_getintprop_default(dp, "height", 900);
	var->xres_virtual = var->xres;
	var->yres_virtual = var->yres;
	var->bits_per_pixel = bpp;
}