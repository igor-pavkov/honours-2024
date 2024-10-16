R_API RBinJavaCPTypeObj *r_bin_java_read_next_constant_pool_item(RBinJavaObj *bin, const ut64 offset, const ut8 *buf, ut64 len) {
	RBinJavaCPTypeMetas *java_constant_info = NULL;
	ut8 tag = 0;
	ut64 buf_sz = 0;
	ut8 *cp_buf = NULL;
	ut32 str_len = 0;
	RBinJavaCPTypeObj *java_obj = NULL;
	tag = buf[offset];
	if (tag > R_BIN_JAVA_CP_METAS_SZ) {
		eprintf ("Invalid tag '%d' at offset 0x%08"PFMT64x "\n", tag, (ut64) offset);
		return NULL;
#if 0
		java_obj = r_bin_java_unknown_cp_new (bin, &tag, 1);
		if (java_obj != NULL && java_obj->metas != NULL) {
			java_obj->file_offset = offset;
			java_obj->loadaddr = bin->loadaddr;
		}
		return NULL; // early error to avoid future overflows
		// return java_obj;
#endif
	}
	java_constant_info = &R_BIN_JAVA_CP_METAS[tag];
	if (java_constant_info->tag == 0 || java_constant_info->tag == 2) {
		return java_obj;
	}
	buf_sz += java_constant_info->len;
	if (java_constant_info->tag == 1) {
		if (offset + 32 < len) {
			str_len = R_BIN_JAVA_USHORT (buf, offset + 1);
			buf_sz += str_len;
		} else {
			return NULL;
		}
	}
	cp_buf = calloc (buf_sz, 1);
	if (!cp_buf) {
		return java_obj;
	}
	if (offset + buf_sz < len) {
		memcpy (cp_buf, (ut8 *) buf + offset, buf_sz);
		IFDBG eprintf("Parsed the tag '%d':%s and create object from offset 0x%08"PFMT64x ".\n", tag, R_BIN_JAVA_CP_METAS[tag].name, offset);
		java_obj = (*java_constant_info->allocs->new_obj)(bin, cp_buf, buf_sz);
		if (java_obj != NULL && java_obj->metas != NULL) {
			java_obj->file_offset = offset;
			// IFDBG eprintf ("java_obj->file_offset = 0x%08"PFMT64x".\n",java_obj->file_offset);
		} else if (!java_obj) {
			eprintf ("Unable to parse the tag '%d' and create valid object.\n", tag);
		} else if (!java_obj->metas) {
			eprintf ("Unable to parse the tag '%d' and create valid object.\n", tag);
		} else {
			eprintf ("Failed to set the java_obj->metas-file_offset for '%d' offset is(0x%08"PFMT64x ").\n", tag, offset);
		}
	}
	free (cp_buf);
	return java_obj;
}