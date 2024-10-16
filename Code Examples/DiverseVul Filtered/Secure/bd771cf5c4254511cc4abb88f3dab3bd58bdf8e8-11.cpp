static void sm7xx_resolution_probe(struct smtcfb_info *sfb)
{
	/* get mode parameter from smtc_scr_info */
	if (smtc_scr_info.lfb_width != 0) {
		sfb->fb->var.xres = smtc_scr_info.lfb_width;
		sfb->fb->var.yres = smtc_scr_info.lfb_height;
		sfb->fb->var.bits_per_pixel = smtc_scr_info.lfb_depth;
		goto final;
	}

	/*
	 * No parameter, default resolution is 1024x768-16.
	 *
	 * FIXME: earlier laptops, such as IBM Thinkpad 240X, has a 800x600
	 * panel, also see the comments about Thinkpad 240X above.
	 */
	sfb->fb->var.xres = SCREEN_X_RES;
	sfb->fb->var.yres = SCREEN_Y_RES_PC;
	sfb->fb->var.bits_per_pixel = SCREEN_BPP;

#ifdef CONFIG_MIPS
	/*
	 * Loongson MIPS netbooks use 1024x600 LCD panels, which is the original
	 * target platform of this driver, but nearly all old x86 laptops have
	 * 1024x768. Lighting 768 panels using 600's timings would partially
	 * garble the display, so we don't want that. But it's not possible to
	 * distinguish them reliably.
	 *
	 * So we change the default to 768, but keep 600 as-is on MIPS.
	 */
	sfb->fb->var.yres = SCREEN_Y_RES_NETBOOK;
#endif

final:
	big_pixel_depth(sfb->fb->var.bits_per_pixel, smtc_scr_info.lfb_depth);
}