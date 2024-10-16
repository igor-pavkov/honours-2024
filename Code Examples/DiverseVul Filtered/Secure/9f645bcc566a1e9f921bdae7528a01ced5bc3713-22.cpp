static void uvesafb_setup_var(struct fb_var_screeninfo *var,
		struct fb_info *info, struct vbe_mode_ib *mode)
{
	struct uvesafb_par *par = info->par;

	var->vmode = FB_VMODE_NONINTERLACED;
	var->sync = FB_SYNC_VERT_HIGH_ACT;

	var->xres = mode->x_res;
	var->yres = mode->y_res;
	var->xres_virtual = mode->x_res;
	var->yres_virtual = (par->ypan) ?
			info->fix.smem_len / mode->bytes_per_scan_line :
			mode->y_res;
	var->xoffset = 0;
	var->yoffset = 0;
	var->bits_per_pixel = mode->bits_per_pixel;

	if (var->bits_per_pixel == 15)
		var->bits_per_pixel = 16;

	if (var->bits_per_pixel > 8) {
		var->red.offset    = mode->red_off;
		var->red.length    = mode->red_len;
		var->green.offset  = mode->green_off;
		var->green.length  = mode->green_len;
		var->blue.offset   = mode->blue_off;
		var->blue.length   = mode->blue_len;
		var->transp.offset = mode->rsvd_off;
		var->transp.length = mode->rsvd_len;
	} else {
		var->red.offset    = 0;
		var->green.offset  = 0;
		var->blue.offset   = 0;
		var->transp.offset = 0;

		var->red.length    = 8;
		var->green.length  = 8;
		var->blue.length   = 8;
		var->transp.length = 0;
	}
}