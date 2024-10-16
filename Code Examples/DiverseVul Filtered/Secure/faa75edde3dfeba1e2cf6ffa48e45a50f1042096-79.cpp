static void lsr_read_opacity(GF_LASeRCodec *lsr, GF_Node *elt)
{
	u32 flag;
	GF_FieldInfo info;
	GF_LSR_READ_INT(lsr, flag, 1, "opacity");
	if (flag) {
		lsr->last_error = gf_node_get_attribute_by_tag(elt, TAG_SVG_ATT_opacity, 1, 0, &info);
		((SVG_Number*)info.far_ptr)->type = SVG_NUMBER_VALUE;
		((SVG_Number*)info.far_ptr)->value = lsr_read_fixed_clamp(lsr, "opacity");
	}

}