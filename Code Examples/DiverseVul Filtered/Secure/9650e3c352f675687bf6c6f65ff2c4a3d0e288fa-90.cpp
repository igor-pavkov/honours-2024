R_API void r_bin_java_print_line_number_table_attr_summary(RBinJavaAttrInfo *attr) {
	RBinJavaLineNumberAttribute *lnattr;
	RListIter *iter, *iter_tmp;
	if (!attr) {
		eprintf ("Attempting to print an invalid RBinJavaAttrInfo *LineNumberTable.\n");
		return;
	}
	Eprintf ("Line Number Table Attribute Information:\n");
	Eprintf ("  Attribute Offset: 0x%08"PFMT64x "\n", attr->file_offset);
	Eprintf ("  Attribute Name Index: %d (%s)\n", attr->name_idx, attr->name);
	Eprintf ("  Attribute Length: %d\n", attr->length);
	r_list_foreach_safe (attr->info.line_number_table_attr.line_number_table, iter, iter_tmp, lnattr) {
		r_bin_java_print_line_number_attr_summary (lnattr);
	}
}