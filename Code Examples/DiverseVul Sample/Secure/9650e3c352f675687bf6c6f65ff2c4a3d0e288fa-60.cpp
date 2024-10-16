R_API void r_bin_java_print_verification_info_summary(RBinJavaVerificationObj *obj) {
	ut8 tag_value = R_BIN_JAVA_STACKMAP_UNKNOWN;
	if (obj == NULL) {
		eprintf ("Attempting to print an invalid RBinJavaVerificationObj*  .\n");
		return;
	}
	if (obj->tag < R_BIN_JAVA_STACKMAP_UNKNOWN) {
		tag_value = obj->tag;
	}
	Eprintf ("Verification Information\n");
	Eprintf ("  Offset: 0x%08"PFMT64x "", obj->file_offset);
	Eprintf ("  Tag Value = 0x%02x\n", obj->tag);
	Eprintf ("  Name = %s\n", R_BIN_JAVA_VERIFICATION_METAS[tag_value].name);
	if (obj->tag == R_BIN_JAVA_STACKMAP_OBJECT) {
		Eprintf ("  Object Constant Pool Index = 0x%x\n", obj->info.obj_val_cp_idx);
	} else if (obj->tag == R_BIN_JAVA_STACKMAP_UNINIT) {
		Eprintf ("  Uninitialized Object offset in code = 0x%x\n", obj->info.uninit_offset);
	}
}