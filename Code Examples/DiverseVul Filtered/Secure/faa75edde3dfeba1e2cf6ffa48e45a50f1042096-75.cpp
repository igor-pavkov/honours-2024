static void lsr_read_anim_repeatCount(GF_LASeRCodec *lsr, GF_Node *n)
{
	u32 val;
	GF_LSR_READ_INT(lsr, val, 1, "has_repeatCount");
	if (val) {
		GF_FieldInfo info;
		lsr->last_error = gf_node_get_attribute_by_tag(n, TAG_SVG_ATT_repeatCount, GF_TRUE, 0, &info);
		GF_LSR_READ_INT(lsr, val, 1, "repeatCount");
		if (val) ((SMIL_RepeatCount*)info.far_ptr)->type = SMIL_REPEATCOUNT_INDEFINITE;
		else {
			((SMIL_RepeatCount*)info.far_ptr)->type = SMIL_REPEATCOUNT_DEFINED;
			((SMIL_RepeatCount*)info.far_ptr)->count = lsr_read_fixed_16_8(lsr, "repeatCount");
		}
	}
}