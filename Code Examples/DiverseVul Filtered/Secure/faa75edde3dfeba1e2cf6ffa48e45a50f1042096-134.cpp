static void lsr_read_additive(GF_LASeRCodec *lsr, GF_Node *n)
{
	Bool v;
	GF_LSR_READ_INT(lsr, v, 1, "has_additive");
	if (v) {
		GF_FieldInfo info;
		lsr->last_error = gf_node_get_attribute_by_tag(n, TAG_SVG_ATT_additive, GF_TRUE, GF_FALSE, &info);
		GF_LSR_READ_INT(lsr, *(SMIL_Additive*)info.far_ptr, 1, "additive");
	}
}