static void sm712_setpalette(int regno, unsigned int red, unsigned int green,
			     unsigned int blue, struct fb_info *info)
{
	/* set bit 5:4 = 01 (write LCD RAM only) */
	smtc_seqw(0x66, (smtc_seqr(0x66) & 0xC3) | 0x10);

	smtc_mmiowb(regno, dac_reg);
	smtc_mmiowb(red >> 10, dac_val);
	smtc_mmiowb(green >> 10, dac_val);
	smtc_mmiowb(blue >> 10, dac_val);
}