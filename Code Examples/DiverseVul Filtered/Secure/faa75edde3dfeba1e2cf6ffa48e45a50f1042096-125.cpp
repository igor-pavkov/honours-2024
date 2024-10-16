static void lsr_read_string_attribute(GF_LASeRCodec *lsr, GF_Node *elt, u32 tag, char *name)
{
	u32 val;
	GF_LSR_READ_INT(lsr, val, 1, name);
	if (val) {
		GF_FieldInfo info;
		lsr->last_error = gf_node_get_attribute_by_tag(elt, tag, GF_TRUE, GF_FALSE, &info);
		lsr_read_byte_align_string(lsr, info.far_ptr, name);
	}
}