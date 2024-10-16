R_API void r_bin_java_print_invokedynamic_cp_summary(RBinJavaCPTypeObj *obj) {
	if (obj == NULL) {
		eprintf ("Attempting to print an invalid RBinJavaCPTypeObj*  RBinJavaCPTypeInvokeDynamic.\n");
		return;
	}
	eprintf ("InvokeDynamic ConstantPool Type (%d) ", obj->metas->ord);
	eprintf ("	Offset: 0x%08"PFMT64x"", obj->file_offset);
	eprintf ("	Bootstrap Method Attr Index = (0x%02x)\n", obj->info.cp_invoke_dynamic.bootstrap_method_attr_index);
	eprintf ("	Bootstrap Name and Type Index = (0x%02x)\n", obj->info.cp_invoke_dynamic.name_and_type_index);
}