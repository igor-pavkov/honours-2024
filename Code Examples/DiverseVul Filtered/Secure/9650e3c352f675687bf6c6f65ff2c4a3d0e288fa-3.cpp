R_API void r_bin_java_print_integer_cp_summary(RBinJavaCPTypeObj *obj) {
	ut8 *b = NULL;
	if (obj == NULL) {
		eprintf ("Attempting to print an invalid RBinJavaCPTypeObj*  Integer.\n");
		return;
	}
	b = obj->info.cp_integer.bytes.raw;
	eprintf ("Integer ConstantPool Type (%d) ", obj->metas->ord);
	eprintf ("	Offset: 0x%08"PFMT64x"", obj->file_offset);
	eprintf ("	bytes = %02x %02x %02x %02x\n", b[0], b[1], b[2], b[3]);
	eprintf ("	integer = %d\n", R_BIN_JAVA_UINT (obj->info.cp_integer.bytes.raw, 0));
}