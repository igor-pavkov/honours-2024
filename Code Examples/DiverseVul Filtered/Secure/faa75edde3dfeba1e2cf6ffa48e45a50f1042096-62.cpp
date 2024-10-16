static void lsr_read_coordinate(GF_LASeRCodec *lsr, SVG_Number *coord, Bool skipable, const char *name)
{
	u32 flag;
	if (skipable) {
		GF_LSR_READ_INT(lsr, flag, 1, name);
		if (!flag) {
			//coord->type = SVG_NUMBER_UNKNOWN;
			//coord->value = 0;
			return;
		}
	}
	coord->type = SVG_NUMBER_VALUE;
	GF_LSR_READ_INT(lsr, flag, lsr->coord_bits, name);
	coord->value = lsr_translate_coords(lsr, flag, lsr->coord_bits);
}