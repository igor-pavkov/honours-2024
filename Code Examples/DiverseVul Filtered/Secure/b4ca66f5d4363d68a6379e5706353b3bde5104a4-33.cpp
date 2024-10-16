RList *r_bin_wasm_get_sections(RBinWasmObj *bin) {
	RList *ret = NULL;
	RBinWasmSection *ptr = NULL;

	if (!bin) {
		return NULL;
	}
	if (bin->g_sections) {
		return bin->g_sections;
	}
	if (!(ret = r_list_newf ((RListFree)wasm_sec_free))) {
		return NULL;
	}
	RBuffer *b = bin->buf;
	ut64 bound = r_buf_size (b) - 1;
	r_buf_seek (b, 8, R_BUF_SET);
	while (r_buf_tell (b) <= bound) {
		if (!(ptr = R_NEW0 (RBinWasmSection))) {
			return ret;
		}
		if (!consume_u7_r (b, bound, &ptr->id)) {
			goto beach;
		}
		if (!consume_u32_r (b, bound, &ptr->size)) {
			goto beach;
		}
		// against spec. TODO: choose criteria for parsing
		if (ptr->size < 1) {
			goto beach;
			// free (ptr);
			// continue;
		}
		ptr->offset = r_buf_tell (b);
		switch (ptr->id) {
		case R_BIN_WASM_SECTION_CUSTOM:
			// eprintf("custom section: 0x%x, ", (ut32)b->cur);
			if (!consume_encoded_name_new (b, bound, &ptr->name_len, &ptr->name)) {
				goto beach;
			}
			break;
		case R_BIN_WASM_SECTION_TYPE:
			// eprintf("section type: 0x%x, ", (ut32)b->cur);
			ptr->name = strdup ("type");
			ptr->name_len = 4;
			break;
		case R_BIN_WASM_SECTION_IMPORT:
			// eprintf("section import: 0x%x, ", (ut32)b->cur);
			ptr->name = strdup ("import");
			ptr->name_len = 6;
			break;
		case R_BIN_WASM_SECTION_FUNCTION:
			// eprintf("section function: 0x%x, ", (ut32)b->cur);
			ptr->name = strdup ("function");
			ptr->name_len = 8;
			break;
		case R_BIN_WASM_SECTION_TABLE:
			// eprintf("section table: 0x%x, ", (ut32)b->cur);
			ptr->name = strdup ("table");
			ptr->name_len = 5;
			break;
		case R_BIN_WASM_SECTION_MEMORY:
			// eprintf("section memory: 0x%x, ", (ut32)b->cur);
			ptr->name = strdup ("memory");
			ptr->name_len = 6;
			break;
		case R_BIN_WASM_SECTION_GLOBAL:
			// eprintf("section global: 0x%x, ", (ut32)b->cur);
			ptr->name = strdup ("global");
			ptr->name_len = 6;
			break;
		case R_BIN_WASM_SECTION_EXPORT:
			// eprintf("section export: 0x%x, ", (ut32)b->cur);
			ptr->name = strdup ("export");
			ptr->name_len = 6;
			break;
		case R_BIN_WASM_SECTION_START:
			// eprintf("section start: 0x%x\n", (ut32)b->cur);
			ptr->name = strdup ("start");
			ptr->name_len = 5;
			break;
		case R_BIN_WASM_SECTION_ELEMENT:
			// eprintf("section element: 0x%x, ", (ut32)b->cur);
			ptr->name = strdup ("element");
			ptr->name_len = 7;
			break;
		case R_BIN_WASM_SECTION_CODE:
			// eprintf("section code: 0x%x, ", (ut32)b->cur);
			ptr->name = strdup ("code");
			ptr->name_len = 4;
			break;
		case R_BIN_WASM_SECTION_DATA:
			// eprintf("section data: 0x%x, ", (ut32)b->cur);
			ptr->name = strdup ("data");
			ptr->name_len = 4;
			break;
		default:
			eprintf ("[wasm] error: unkown section id: %d\n", ptr->id);
			r_buf_seek (b, ptr->size - 1, R_BUF_CUR);
			continue;
		}
		if (ptr->offset + (ut64)ptr->size - 1 > bound) {
			// TODO: Better error handling here
			ut32 diff = ptr->size - (bound + 1 - ptr->offset);
			eprintf ("[wasm] Artificially reducing size of section %s by 0x%x bytes so it fits in the file\n", ptr->name, diff);
			ptr->size -= diff;
		}
		ptr->payload_data = r_buf_tell (b);
		ptr->payload_len = ptr->size - (ptr->payload_data - ptr->offset);
		if (ptr->payload_len > ptr->size) {
			goto beach;
		}
		r_buf_seek (b, ptr->payload_len, R_BUF_CUR);
		if (!r_list_append (ret, ptr)) {
			free (ptr);
			// should it jump to beach?
		}
		ptr = NULL;
	}
	bin->g_sections = ret;
	return ret;
beach:
	eprintf ("[wasm] error: beach sections\n");
	free (ptr);
	return ret;
}