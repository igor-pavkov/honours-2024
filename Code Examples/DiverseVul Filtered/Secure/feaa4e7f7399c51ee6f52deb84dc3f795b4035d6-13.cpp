static ut64 r_rebase_offset_to_paddr(RKernelCacheObj *obj, struct section_t *sections, ut64 offset) {
	ut64 vaddr = obj->rebase_info->kernel_base + offset;
	int i = 0;
	for (; !sections[i].last; i++) {
		if (sections[i].addr <= vaddr && vaddr < (sections[i].addr + sections[i].vsize)) {
			return sections[i].offset + (vaddr - sections[i].addr);
		}
	}
	return offset;
}