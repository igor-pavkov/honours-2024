static void lsr_read_href(GF_LASeRCodec *lsr, GF_Node *n)
{
	Bool has_href;
	GF_LSR_READ_INT(lsr, has_href, 1, "has_href");
	if (has_href) {
		GF_FieldInfo info;
		lsr->last_error = gf_node_get_attribute_by_tag(n, TAG_XLINK_ATT_href, GF_TRUE, GF_FALSE, &info);
		lsr_read_any_uri(lsr, info.far_ptr, "href");
	}
}