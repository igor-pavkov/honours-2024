static void lsr_read_fixed_16_8i(GF_LASeRCodec *lsr, SVG_Number *n, const char *name)
{
	s32 val;
	GF_LSR_READ_INT(lsr, val, 1, name);
	if (val) {
		n->type=SVG_NUMBER_INHERIT;
	} else {
		n->type=SVG_NUMBER_VALUE;
		n->value = lsr_read_fixed_16_8(lsr, name);
	}
}