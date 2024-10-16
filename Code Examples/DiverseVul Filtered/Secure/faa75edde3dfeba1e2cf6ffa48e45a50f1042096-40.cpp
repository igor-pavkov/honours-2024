static void lsr_read_stroke(GF_LASeRCodec *lsr, GF_Node *n)
{
	Bool has_stroke;
	GF_LSR_READ_INT(lsr, has_stroke, 1, "has_stroke");
	if (has_stroke) {
		GF_FieldInfo info;
		lsr->last_error = gf_node_get_attribute_by_tag(n, TAG_SVG_ATT_stroke, GF_TRUE, GF_FALSE, &info);
		lsr_read_paint(lsr, info.far_ptr, "stroke");
	}
}