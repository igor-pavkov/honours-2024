R_API void r_bin_java_print_float_cp_summary(RBinJavaCPTypeObj *obj) {
	ut8 *b = NULL;
	if (obj == NULL) {
		eprintf ("Attempting to print an invalid RBinJavaCPTypeObj*  Double.\n");
		return;
	}
	b = obj->info.cp_float.bytes.raw;
	Eprintf ("Float ConstantPool Type (%d) ", obj->metas->ord);
	Eprintf ("  Offset: 0x%08"PFMT64x "", obj->file_offset);
	Eprintf ("  Bytes = %02x %02x %02x %02x\n", b[0], b[1], b[2], b[3]);
	Eprintf ("  Float = %f\n", R_BIN_JAVA_FLOAT (obj->info.cp_float.bytes.raw, 0));
}