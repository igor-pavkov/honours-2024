static void lsr_read_lsr_enabled(GF_LASeRCodec *lsr, GF_Node *elt)
{
	u32 err;
	GF_LSR_READ_INT(lsr, err, 1, "enabled");
	if (err) {
		GF_FieldInfo info;
		lsr->last_error = gf_node_get_attribute_by_tag(elt, TAG_LSR_ATT_enabled, 1, 0, &info);
		*(SVG_Boolean*)info.far_ptr = 1;
	}
}