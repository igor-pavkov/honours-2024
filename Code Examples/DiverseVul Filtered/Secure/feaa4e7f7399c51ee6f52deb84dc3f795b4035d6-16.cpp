static int kexts_sort_vaddr_func(const void *a, const void *b) {
	RKext *A = (RKext *) a;
	RKext *B = (RKext *) b;
	int vaddr_compare = A->vaddr - B->vaddr;
	if (vaddr_compare == 0) {
		return A->text_range.size - B->text_range.size;
	}
	return vaddr_compare;
}