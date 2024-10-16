static Fixed lsr_read_fixed_16_8(GF_LASeRCodec *lsr, const char *name)
{
	u32 val;
	GF_LSR_READ_INT(lsr, val, 24, name);
	if (val & (1<<23)) {
		s32 res = val - (1<<24);
#ifdef GPAC_FIXED_POINT
		return res*256;
#else
		return INT2FIX(res) / 256;
#endif
	} else {
#ifdef GPAC_FIXED_POINT
		return val*256;
#else
		return INT2FIX(val) / 256;
#endif
	}
}