R_API void r_bin_java_print_utf8_cp_summary(RBinJavaCPTypeObj *obj) {
	if (obj == NULL) {
		eprintf ("Attempting to print an invalid RBinJavaCPTypeObj*  Utf8.\n");
		return;
	}
	char *str = convert_string ((const char *) obj->info.cp_utf8.bytes, obj->info.cp_utf8.length);
	eprintf ("UTF8 ConstantPool Type (%d) ", obj->metas->ord);
	eprintf ("	Offset: 0x%08"PFMT64x"", obj->file_offset);
	eprintf ("	length = %d\n", obj->info.cp_utf8.length);
	eprintf ("	utf8 = %s\n", str);
	free (str);
}