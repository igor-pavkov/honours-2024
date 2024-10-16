static void lsr_read_line_increment_type(GF_LASeRCodec *lsr, SVG_Number *li, const char *name)
{
	u32 val;
	GF_LSR_READ_INT(lsr, val, 1, "choice");
	if (val==1) {
		GF_LSR_READ_INT(lsr, val, 1, "type");
		if (val==1) li->type=SVG_NUMBER_INHERIT;
		else li->type=SVG_NUMBER_AUTO;
	} else {
		li->value = lsr_read_fixed_16_8(lsr, "line-increment-value");
	}
}