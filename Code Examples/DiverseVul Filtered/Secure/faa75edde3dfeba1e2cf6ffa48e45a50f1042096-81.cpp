static void lsr_read_repeat_duration(GF_LASeRCodec *lsr, GF_Node *n)
{
	u32 flag;
	GF_LSR_READ_INT(lsr, flag, 1, "has_repeatDur");
	if (flag) {
		GF_FieldInfo info;
		lsr->last_error = gf_node_get_attribute_by_tag(n, TAG_SVG_ATT_repeatDur, GF_TRUE, 0, &info);
		GF_LSR_READ_INT(lsr, flag, 1, "choice");

		if (flag) {
			((SMIL_Duration *)info.far_ptr)->type = SMIL_DURATION_INDEFINITE;
		} else {
			((SMIL_Duration *)info.far_ptr)->clock_value = (Double) lsr_read_vluimsbf5(lsr, "value");
			((SMIL_Duration *)info.far_ptr)->clock_value /= lsr->time_resolution;
			((SMIL_Duration *)info.far_ptr)->type = SMIL_DURATION_DEFINED;
		}
	}
}