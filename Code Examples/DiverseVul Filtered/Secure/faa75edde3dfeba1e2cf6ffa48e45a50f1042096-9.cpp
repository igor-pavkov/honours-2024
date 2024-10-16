static void lsr_read_duration_ex(GF_LASeRCodec *lsr, GF_Node *n, u32 tag, SMIL_Duration *smil, const char *name, Bool skipable)
{
	GF_FieldInfo info;
	u32 val = 1;

	if (skipable) {
		GF_LSR_READ_INT(lsr, val, 1, name);
		if (!val) return;
	}
	if (!smil) {
		lsr->last_error = gf_node_get_attribute_by_tag(n, tag, GF_TRUE, GF_FALSE, &info);
		if (lsr->last_error) return;
		smil = (SMIL_Duration *)info.far_ptr;
	}
	smil->type = 0;
	smil->clock_value=0;

	GF_LSR_READ_INT(lsr, val, 1, "choice");
	if (val) {
		GF_LSR_READ_INT(lsr, smil->type, 2, "time");
	} else {
		Bool sign;
		u32 now;
		GF_LSR_READ_INT(lsr, sign, 1, "sign");
		now = lsr_read_vluimsbf5(lsr, "value");
		smil->clock_value = now;
		smil->clock_value /= lsr->time_resolution;
		if (sign) smil->clock_value *= -1;
		smil->type = SMIL_DURATION_DEFINED;
	}
}