static void lsr_read_content_type(GF_LASeRCodec *lsr, GF_Node *n)
{
	u32 flag;
	GF_LSR_READ_INT(lsr, flag, 1, "hasType");
	if (flag) {
		GF_FieldInfo info;
		lsr->last_error = gf_node_get_attribute_by_tag(n, TAG_SVG_ATT_type, GF_TRUE, 0, &info);
		lsr_read_byte_align_string(lsr, info.far_ptr, "type");
	}
}