R_API void r_bin_java_print_exceptions_attr_summary(RBinJavaAttrInfo *attr) {
	ut32 i = 0;
	if (!attr) {
		eprintf ("Attempting to print an invalid RBinJavaAttrInfo *Exceptions.\n");
		return;
	}
	Eprintf ("Exceptions Attribute Information:\n");
	Eprintf ("  Attribute Offset: 0x%08"PFMT64x "\n", attr->file_offset);
	Eprintf ("  Attribute Name Index: %d (%s)\n", attr->name_idx, attr->name);
	Eprintf ("  Attribute Length: %d\n", attr->length);
	for (i = 0; i < attr->info.exceptions_attr.number_of_exceptions; i++) {
		Eprintf ("  Exceptions Attribute Index[%d]: %d\n", i, attr->info.exceptions_attr.exception_idx_table[i]);
	}
}