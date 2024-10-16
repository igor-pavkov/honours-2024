static int uvesafb_setcolreg(unsigned regno, unsigned red, unsigned green,
		unsigned blue, unsigned transp,
		struct fb_info *info)
{
	struct uvesafb_pal_entry entry;
	int shift = 16 - dac_width;
	int err = 0;

	if (regno >= info->cmap.len)
		return -EINVAL;

	if (info->var.bits_per_pixel == 8) {
		entry.red   = red   >> shift;
		entry.green = green >> shift;
		entry.blue  = blue  >> shift;
		entry.pad   = 0;

		err = uvesafb_setpalette(&entry, 1, regno, info);
	} else if (regno < 16) {
		switch (info->var.bits_per_pixel) {
		case 16:
			if (info->var.red.offset == 10) {
				/* 1:5:5:5 */
				((u32 *) (info->pseudo_palette))[regno] =
						((red   & 0xf800) >>  1) |
						((green & 0xf800) >>  6) |
						((blue  & 0xf800) >> 11);
			} else {
				/* 0:5:6:5 */
				((u32 *) (info->pseudo_palette))[regno] =
						((red   & 0xf800)      ) |
						((green & 0xfc00) >>  5) |
						((blue  & 0xf800) >> 11);
			}
			break;

		case 24:
		case 32:
			red   >>= 8;
			green >>= 8;
			blue  >>= 8;
			((u32 *)(info->pseudo_palette))[regno] =
				(red   << info->var.red.offset)   |
				(green << info->var.green.offset) |
				(blue  << info->var.blue.offset);
			break;
		}
	}
	return err;
}