static void free_import_entry(RBinWasmImportEntry *entry) {
	if (entry) {
		free (entry->module_str);
		free (entry->field_str);
		free (entry);
	}
}