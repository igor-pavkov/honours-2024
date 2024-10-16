R_API ut64 r_bin_file_get_vaddr(RBinFile *bf, ut64 paddr, ut64 vaddr) {
	r_return_val_if_fail (bf && bf->o, paddr);
	if (bf->o->info && bf->o->info->has_va) {
		return binobj_a2b (bf->o, vaddr);
	}
	return paddr;
}