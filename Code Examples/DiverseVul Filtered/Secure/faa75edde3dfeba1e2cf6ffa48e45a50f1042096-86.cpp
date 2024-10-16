static void lsr_read_coord_list(GF_LASeRCodec *lsr, GF_Node *elt, u32 tag, const char *name)
{
	GF_FieldInfo info;
	u32 i, count;
	GF_LSR_READ_INT(lsr, count, 1, name);
	if (!count) return;
	count = lsr_read_vluimsbf5(lsr, "nb_coords");
	if (!count) return;

	lsr->last_error = gf_node_get_attribute_by_tag(elt, tag, GF_TRUE, 0, &info);

	for (i=0; i<count; i++) {
		u32 res;
		SVG_Coordinate *f;
		GF_SAFEALLOC(f, SVG_Coordinate );
		if (!f) {
			lsr->last_error = GF_OUT_OF_MEM;
			return;
		}
		GF_LSR_READ_INT(lsr, res, lsr->coord_bits, name);
		f->value = lsr_translate_coords(lsr, res, lsr->coord_bits);
		gf_list_add(*(SVG_Coordinates*)info.far_ptr, f);
		if (lsr->last_error) return;
	}
}