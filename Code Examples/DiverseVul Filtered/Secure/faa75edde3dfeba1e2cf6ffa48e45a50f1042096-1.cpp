static Fixed *lsr_read_fraction_12_item(GF_LASeRCodec *lsr)
{
	u32 flag;
	Fixed *f;
	GF_SAFEALLOC(f, Fixed);
	if (!f) {
		lsr->last_error = GF_OUT_OF_MEM;
		return NULL;
	}
	GF_LSR_READ_INT(lsr, flag, 1, "hasShort");
	if (flag) {
		GF_LSR_READ_INT(lsr, flag, 1, "isZero");
		if (flag) *f = 0;
		else *f = FIX_ONE;
	} else {
		u32 v;
		GF_LSR_READ_INT(lsr, v, 12, "val");
		*f = INT2FIX(v) / 4096/*(1<<12)*/;
	}
	return f;
}