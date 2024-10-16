static void lsr_read_sync_tolerance(GF_LASeRCodec *lsr, GF_Node *n)
{
	u32 flag;
	GF_LSR_READ_INT(lsr, flag, 1, "syncTolerance");
	if (flag) {
		GF_FieldInfo info;
		GF_LSR_READ_INT(lsr, flag, 1, "syncTolerance");
		lsr->last_error = gf_node_get_attribute_by_tag(n, TAG_SVG_ATT_syncTolerance, GF_TRUE, 0, &info);
		if (flag) ((SMIL_SyncTolerance *)info.far_ptr)->type = SMIL_SYNCTOLERANCE_DEFAULT;
		else {
			u32 v = lsr_read_vluimsbf5(lsr, "value");
			((SMIL_SyncTolerance *)info.far_ptr)->value = INT2FIX(v);
			((SMIL_SyncTolerance *)info.far_ptr)->value /= lsr->time_resolution;
		}
	}
}