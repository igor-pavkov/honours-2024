static void lsr_read_clip_time(GF_LASeRCodec *lsr, GF_Node *elt, u32 tag, const char *name)
{
	GF_FieldInfo info;
	u32 flag;
	GF_LSR_READ_INT(lsr, flag, 1, name);
	if (flag) {
		lsr->last_error = gf_node_get_attribute_by_tag(elt, tag, 1, 0, &info);
		GF_LSR_READ_INT(lsr, flag, 1, "isEnum");
		if (!flag) {
			GF_LSR_READ_INT(lsr, flag, 1, "sign");
			flag = lsr_read_vluimsbf5(lsr, "val");
			*((SVG_Clock *)info.far_ptr) = flag;
			*((SVG_Clock *)info.far_ptr) /= lsr->time_resolution;
		}
	}
}