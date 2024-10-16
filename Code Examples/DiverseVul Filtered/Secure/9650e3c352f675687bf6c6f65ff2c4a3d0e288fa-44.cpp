R_API char *r_bin_java_print_unknown_cp_stringify(RBinJavaCPTypeObj *obj) {
	ut32 size = 255;
	char *value = malloc (size);
	if (value) {
		memset (value, 0, size);
		snprintf (value, size, "%d.0x%04"PFMT64x ".%s",
			obj->metas->ord, obj->file_offset + obj->loadaddr, ((RBinJavaCPTypeMetas *) obj->metas->type_info)->name);
	}
	return value;
}