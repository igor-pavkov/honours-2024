R_API ut64 r_binfile_get_vaddr(RBinFile *binfile, ut64 paddr, ut64 vaddr) {
	int use_va = 0;
	if (binfile && binfile->o && binfile->o->info) {
		use_va = binfile->o->info->has_va;
	}
	return use_va? binobj_a2b (binfile->o, vaddr): paddr;
}