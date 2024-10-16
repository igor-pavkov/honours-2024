R_API void r_bin_java_print_classref_cp_summary(RBinJavaCPTypeObj *obj) {
	if (obj == NULL) {
		eprintf ("Attempting to print an invalid RBinJavaCPTypeObj*  ClassRef.\n");
		return;
	}
	eprintf ("ClassRef ConstantPool Type (%d) ", obj->metas->ord);
	eprintf ("	Offset: 0x%08"PFMT64x"", obj->file_offset);
	eprintf ("	Name Index = %d\n", obj->info.cp_class.name_idx);
}