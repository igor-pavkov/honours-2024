R_API void r_bin_bind(RBin *bin, RBinBind *b) {
	if (b) {
		b->bin = bin;
		b->get_offset = getoffset;
		b->get_name = getname;
		b->get_sections = r_bin_get_sections;
		b->get_vsect_at = _get_vsection_at;
	}
}