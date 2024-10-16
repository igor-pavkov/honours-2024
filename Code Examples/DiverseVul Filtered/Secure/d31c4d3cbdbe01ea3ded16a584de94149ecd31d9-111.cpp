static RBinSection* _get_vsection_at(RBin *bin, ut64 vaddr) {
	RBinObject *cur = r_bin_object_get_cur (bin);
	return r_bin_get_section_at (cur, vaddr, true);
}