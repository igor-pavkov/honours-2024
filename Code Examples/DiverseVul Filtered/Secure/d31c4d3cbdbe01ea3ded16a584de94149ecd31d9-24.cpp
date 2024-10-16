R_API RBinField *r_bin_field_new(ut64 paddr, ut64 vaddr, int size, const char *name, const char *comment, const char *format) {
	RBinField *ptr;
	if (!(ptr = R_NEW0 (RBinField))) {
		return NULL;
	}
	ptr->name = strdup (name);
	ptr->comment = (comment && *comment)? strdup (comment): NULL;
	ptr->format = (format && *format)? strdup (format): NULL;
	ptr->paddr = paddr;
	ptr->size = size;
//	ptr->visibility = ???
	ptr->vaddr = vaddr;
	return ptr;
}