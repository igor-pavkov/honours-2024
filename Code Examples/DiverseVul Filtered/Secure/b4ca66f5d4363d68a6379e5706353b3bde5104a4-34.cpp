static void free_code_entry(RBinWasmCodeEntry *ptr) {
	if (ptr) {
		free (ptr->locals);
		free (ptr);
	}
}