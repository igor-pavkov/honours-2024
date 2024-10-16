static void lsr_read_sync_reference(GF_LASeRCodec *lsr, GF_Node *n)
{
	u32 flag;
	GF_LSR_READ_INT(lsr, flag, 1, "hasSyncReference");
	if (flag) {
		GF_FieldInfo info;
		lsr->last_error = gf_node_get_attribute_by_tag(n, TAG_SVG_ATT_syncReference, GF_TRUE, 0, &info);
		lsr_read_any_uri(lsr, info.far_ptr, "syncReference");
	}
}