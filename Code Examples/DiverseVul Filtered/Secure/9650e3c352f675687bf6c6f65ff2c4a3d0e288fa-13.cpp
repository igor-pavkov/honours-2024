R_API ut64 r_bin_java_parse_cp_pool(RBinJavaObj *bin, const ut64 offset, const ut8 *buf, const ut64 len) {
	int ord = 0;
	ut64 adv = 0;
	RBinJavaCPTypeObj *obj = NULL;
	const ut8 *cp_buf = buf + offset;
	r_list_free (bin->cp_list);
	bin->cp_list = r_list_newf (r_bin_java_constant_pool);
	bin->cp_offset = offset;
	memcpy ((char *) &bin->cp_count, cp_buf, 2);
	bin->cp_count = R_BIN_JAVA_USHORT (cp_buf, 0) - 1;
	adv += 2;
	IFDBG eprintf("ConstantPoolCount %d\n", bin->cp_count);
	r_list_append (bin->cp_list, r_bin_java_get_java_null_cp ());
	for (ord = 1, bin->cp_idx = 0; bin->cp_idx < bin->cp_count && adv < len; ord++, bin->cp_idx++) {
		obj = r_bin_java_read_next_constant_pool_item (bin, offset + adv, buf, len);
		if (obj) {
			// IFDBG eprintf ("SUCCESS Read ConstantPoolItem %d\n", i);
			obj->metas->ord = ord;
			obj->idx = ord;
			r_list_append (bin->cp_list, obj);
			if (obj->tag == R_BIN_JAVA_CP_LONG || obj->tag == R_BIN_JAVA_CP_DOUBLE) {
				// i++;
				ord++;
				bin->cp_idx++;
				r_list_append (bin->cp_list, &R_BIN_JAVA_NULL_TYPE);
			}

			IFDBG ((RBinJavaCPTypeMetas *) obj->metas->type_info)->allocs->print_summary (obj);
			adv += ((RBinJavaCPTypeMetas *) obj->metas->type_info)->allocs->calc_size (obj);
			if (offset + adv > len) {
				eprintf ("[X] r_bin_java: Error unable to parse remainder of classfile after Constant Pool Object: %d.\n", ord);
				break;
			}
		} else {
			IFDBG eprintf("Failed to read ConstantPoolItem %d\n", bin->cp_idx);
			break;
		}
	}
	// Update the imports
	r_bin_java_set_imports (bin);
	bin->cp_size = adv;
	return bin->cp_size;
}