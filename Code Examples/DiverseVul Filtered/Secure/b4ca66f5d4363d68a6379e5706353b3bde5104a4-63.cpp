static RBinWasmExportEntry *parse_export_entry(RBinWasmObj *bin, ut64 bound, ut32 index) {
	RBuffer *b = bin->buf;
	RBinWasmExportEntry *export = R_NEW0 (RBinWasmExportEntry);
	if (export) {
		export->sec_i = index;
		export->file_offset = r_buf_tell (b);
		if (!consume_encoded_name_new (b, bound, &export->field_len, &export->field_str)) {
			goto beach;
		}
		if (!consume_u7_r (b, bound, &export->kind)) {
			goto beach;
		}
		if (!consume_u32_r (b, bound, &export->index)) {
			goto beach;
		}
	}
	return export;
beach:
	free_export_entry (export);
	return NULL;
}