R_API RList * r_bin_patch_relocs(RBin *bin) {
	static bool first = true;
	RBinObject *o = r_bin_cur_object (bin);
	if (!o) {
		return NULL;
	}
	// r_bin_object_set_items set o->relocs but there we don't have access
	// to io
	// so we need to be run from bin_relocs, free the previous reloc and get
	// the patched ones
	if (first && o->plugin && o->plugin->patch_relocs) {
		RList *tmp = o->plugin->patch_relocs (bin);
		first = false;
		if (!tmp) {
			return o->relocs;
		}
		r_list_free (o->relocs);
		o->relocs = tmp;
		REBASE_PADDR (o, o->relocs, RBinReloc);
		first = false;
		return o->relocs;
	}
	return o->relocs;
}