static void *parse_import_entry(RBinWasmObj *bin, ut64 bound, ut32 index) {
	RBuffer *b = bin->buf;
	RBinWasmImportEntry *ptr = R_NEW0 (RBinWasmImportEntry);
	if (!ptr) {
		return NULL;
	}
	ptr->sec_i = index;
	ptr->file_offset = r_buf_tell (b);

	if (!consume_encoded_name_new (b, bound, &ptr->module_len, &ptr->module_str)) {
		goto beach;
	}

	if (!consume_encoded_name_new (b, bound, &ptr->field_len, &ptr->field_str)) {
		goto beach;
	}

	if (!consume_u7_r (b, bound, &ptr->kind)) {
		goto beach;
	}
	switch (ptr->kind) {
	case 0: // Function
		if (!consume_u32_r (b, bound, &ptr->type_f)) {
			goto beach;
		}
		break;
	case 1: // Table
		if (!consume_s7_r (b, bound, (st8 *)&ptr->type_t.elem_type)) {
			goto beach;
		}
		if (!consume_limits_r (b, bound, &ptr->type_t.limits)) {
			goto beach;
		}
		break;
	case 2: // Memory
		if (!consume_limits_r (b, bound, &ptr->type_m.limits)) {
			goto beach;
		}
		break;
	case 3: // Global
		if (!consume_s7_r (b, bound, (st8 *)&ptr->type_g.content_type)) {
			goto beach;
		}
		if (!consume_u1_r (b, bound, (ut8 *)&ptr->type_g.mutability)) {
			goto beach;
		}
		break;
	default:
		goto beach;
	}
	return ptr;

beach:
	free (ptr);
	return NULL;
}