static void lsr_read_coordinate_ptr(GF_LASeRCodec *lsr, GF_Node *n, u32 tag, Bool skipable, const char *name)
{
	u32 flag;
	GF_FieldInfo info;
	if (skipable) {
		GF_LSR_READ_INT(lsr, flag, 1, name);
		if (!flag) return;
	}
	lsr->last_error = gf_node_get_attribute_by_tag(n, tag, GF_TRUE, 0, &info);

	((SVG_Number*)info.far_ptr)->type = SVG_NUMBER_VALUE;
	GF_LSR_READ_INT(lsr, flag, lsr->coord_bits, name);
	((SVG_Number*)info.far_ptr)->value = lsr_translate_coords(lsr, flag, lsr->coord_bits);
}