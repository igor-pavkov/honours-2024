static Fixed lsr_read_fixed_clamp(GF_LASeRCodec *lsr, const char *name)
{
	s32 val;
	GF_LSR_READ_INT(lsr, val, 8, name);
	return INT2FIX(val) / 255;
}