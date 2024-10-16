R_API void r_bin_java_print_code_attr_summary(RBinJavaAttrInfo *attr) {
	RListIter *iter = NULL, *iter_tmp = NULL;
	RBinJavaExceptionEntry *exc_entry = NULL;
	RBinJavaAttrInfo *_attr = NULL;
	if (!attr) {
		eprintf ("Attempting to print an invalid RBinJavaAttrInfo *Code.\n");
		return;
	}
	Eprintf ("Code Attribute Information:\n");
	Eprintf ("  Attribute Offset: 0x%08"PFMT64x "\n", attr->file_offset);
	Eprintf ("  Attribute Name Index: %d (%s)\n", attr->name_idx, attr->name);
	Eprintf ("  Attribute Length: %d, Attribute Count: %d\n", attr->length, attr->info.code_attr.attributes_count);
	Eprintf ("    Max Stack: %d\n", attr->info.code_attr.max_stack);
	Eprintf ("    Max Locals: %d\n", attr->info.code_attr.max_locals);
	Eprintf ("    Code Length: %d\n", attr->info.code_attr.code_length);
	Eprintf ("    Code At Offset: 0x%08"PFMT64x "\n", (ut64) attr->info.code_attr.code_offset);
	Eprintf ("Code Attribute Exception Table Information:\n");
	Eprintf ("  Exception Table Length: %d\n", attr->info.code_attr.exception_table_length);
	if (attr->info.code_attr.exception_table) {
		// Delete the attr entries
		r_list_foreach_safe (attr->info.code_attr.exception_table, iter, iter_tmp, exc_entry) {
			r_bin_java_print_code_exceptions_attr_summary (exc_entry);
		}
	}
	Eprintf ("  Implicit Method Stack Frame:\n");
	r_bin_java_print_stack_map_frame_summary (attr->info.code_attr.implicit_frame);
	Eprintf ("Code Attribute Attributes Information:\n");
	if (attr->info.code_attr.attributes && attr->info.code_attr.attributes_count > 0) {
		Eprintf ("  Code Attribute Attributes Count: %d\n", attr->info.code_attr.attributes_count);
		r_list_foreach_safe (attr->info.code_attr.attributes, iter, iter_tmp, _attr) {
			r_bin_java_print_attr_summary (_attr);
		}
	}
}