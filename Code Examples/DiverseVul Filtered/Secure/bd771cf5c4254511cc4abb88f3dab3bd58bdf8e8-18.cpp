static int smtc_setcolreg(unsigned int regno, unsigned int red,
			  unsigned int green, unsigned int blue,
			  unsigned int trans, struct fb_info *info)
{
	struct smtcfb_info *sfb;
	u32 val;

	sfb = info->par;

	if (regno > 255)
		return 1;

	switch (sfb->fb->fix.visual) {
	case FB_VISUAL_DIRECTCOLOR:
	case FB_VISUAL_TRUECOLOR:
		/*
		 * 16/32 bit true-colour, use pseudo-palette for 16 base color
		 */
		if (regno >= 16)
			break;
		if (sfb->fb->var.bits_per_pixel == 16) {
			u32 *pal = sfb->fb->pseudo_palette;

			val = chan_to_field(red, &sfb->fb->var.red);
			val |= chan_to_field(green, &sfb->fb->var.green);
			val |= chan_to_field(blue, &sfb->fb->var.blue);
			pal[regno] = pal_rgb(red, green, blue, val);
		} else {
			u32 *pal = sfb->fb->pseudo_palette;

			val = chan_to_field(red, &sfb->fb->var.red);
			val |= chan_to_field(green, &sfb->fb->var.green);
			val |= chan_to_field(blue, &sfb->fb->var.blue);
			pal[regno] = big_swap(val);
		}
		break;

	case FB_VISUAL_PSEUDOCOLOR:
		/* color depth 8 bit */
		sm712_setpalette(regno, red, green, blue, info);
		break;

	default:
		return 1;	/* unknown type */
	}

	return 0;
}