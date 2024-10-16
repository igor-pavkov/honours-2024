R_API void r_bin_java_print_local_variable_table_attr_summary(RBinJavaAttrInfo *attr) {
	RBinJavaLocalVariableAttribute *lvattr;
	RListIter *iter, *iter_tmp;
	if (attr == NULL) {
		eprintf ("Attempting to print an invalid RBinJavaAttrInfo *LocalVariableTable.\n");
		return;
	}
	Eprintf ("Local Variable Table Attribute Information:\n");
	Eprintf ("  Attribute Offset: 0x%08"PFMT64x "\n", attr->file_offset);
	Eprintf ("  Attribute Name Index: %d (%s)\n", attr->name_idx, attr->name);
	Eprintf ("  Attribute Length: %d\n", attr->length);
	r_list_foreach_safe (attr->info.local_variable_table_attr.local_variable_table, iter, iter_tmp, lvattr) {
		r_bin_java_print_local_variable_attr_summary (lvattr);
	}
}