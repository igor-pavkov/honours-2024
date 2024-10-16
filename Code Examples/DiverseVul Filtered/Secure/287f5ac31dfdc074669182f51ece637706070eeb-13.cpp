static int handler_RGBColor(deark *c, lctx *d, i64 opcode, i64 data_pos, i64 *bytes_used)
{
	unsigned int clr16[3];
	u8 clr8[3];
	u32 clr;
	char csamp[16];
	i64 pos = data_pos;
	i64 k;

	for(k=0; k<3; k++) {
		clr16[k] = (unsigned int)de_getu16be_p(&pos);
		clr8[k] = (u8)(clr16[k]>>8);
	}
	clr = DE_MAKE_RGB(clr8[0], clr8[1], clr8[2]);
	de_get_colorsample_code(c, clr, csamp, sizeof(csamp));
	de_dbg(c, "color: (0x%04x,0x%04x,0x%04x)%s", clr16[0], clr16[1], clr16[2], csamp);
	return 1;
}