R_API void r_bin_java_print_methodhandle_cp_summary(RBinJavaCPTypeObj *obj) {
	ut8 ref_kind;
	if (obj == NULL) {
		eprintf ("Attempting to print an invalid RBinJavaCPTypeObj*  RBinJavaCPTypeMethodHandle.\n");
		return;
	}
	ref_kind = obj->info.cp_method_handle.reference_kind;
	eprintf ("MethodHandle ConstantPool Type (%d) ", obj->metas->ord);
	eprintf ("	Offset: 0x%08"PFMT64x"", obj->file_offset);
	eprintf ("	Reference Kind = (0x%02x) %s\n", ref_kind, R_BIN_JAVA_REF_METAS[ref_kind].name);
	eprintf ("	Reference Index = %d\n", obj->info.cp_method_handle.reference_index);
}