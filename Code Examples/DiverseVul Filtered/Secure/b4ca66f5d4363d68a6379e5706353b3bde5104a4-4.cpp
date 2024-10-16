static void free_export_entry(RBinWasmExportEntry *entry) {
	if (entry) {
		free (entry->field_str);
		free (entry);
	}
}