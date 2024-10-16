static void lsr_read_anim_fill(GF_LASeRCodec *lsr, GF_Node *n)
{
	u32 val;

	GF_LSR_READ_INT(lsr, val, 1, "has_smil_fill");
	if (val) {
		GF_FieldInfo info;
		lsr->last_error = gf_node_get_attribute_by_tag(n, TAG_SVG_ATT_smil_fill, GF_TRUE, 0, &info);
		/*enumeration freeze{0} remove{1}*/
		GF_LSR_READ_INT(lsr, val, 1, "smil_fill");
		*(SMIL_Fill*)info.far_ptr = val ? SMIL_FILL_REMOVE : SMIL_FILL_FREEZE;
	}
}