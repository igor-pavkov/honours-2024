RList *r_bin_le_get_entrypoints(r_bin_le_obj_t *bin) {
	RList *l = r_list_newf ((RListFree)free);
	if (!l) {
		return NULL;
	}
	RBinAddr *entry = R_NEW0 (RBinAddr);
	if (entry) {
		if ((bin->header->startobj - 1) < bin->header->objcnt){
			entry->vaddr = (ut64)bin->objtbl[bin->header->startobj - 1].reloc_base_addr + bin->header->eip;
		}
	}
	r_list_append (l, entry);

	return l;
}