static void lsr_read_sync_behavior(GF_LASeRCodec *lsr, GF_Node *n)
{
	u32 flag;
	GF_LSR_READ_INT(lsr, flag, 1, "syncBehavior");
	if (flag) {
		GF_FieldInfo info;
		GF_LSR_READ_INT(lsr, flag, 2, "syncBehavior");
		lsr->last_error = gf_node_get_attribute_by_tag(n, TAG_SVG_ATT_syncBehavior, GF_TRUE, 0, &info);
		*(SMIL_SyncBehavior*)info.far_ptr = flag + 1;
	}
}