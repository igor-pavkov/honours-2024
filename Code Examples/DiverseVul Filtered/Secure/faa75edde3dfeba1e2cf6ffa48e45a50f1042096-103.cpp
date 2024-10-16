static void lsr_read_fill(GF_LASeRCodec *lsr, GF_Node *n)
{
	Bool has_fill;
	GF_LSR_READ_INT(lsr, has_fill, 1, "fill");
	if (has_fill) {
		GF_FieldInfo info;
		lsr->last_error = gf_node_get_attribute_by_tag(n, TAG_SVG_ATT_fill, GF_TRUE, GF_FALSE, &info);
		lsr_read_paint(lsr, info.far_ptr, "fill");
	}
}