static void free_type_entry(RBinWasmTypeEntry *ptr) {
	if (ptr) {
		free_type_vec (ptr->args);
		free_type_vec (ptr->rets);
		free (ptr->to_str);
		free (ptr);
	}
}