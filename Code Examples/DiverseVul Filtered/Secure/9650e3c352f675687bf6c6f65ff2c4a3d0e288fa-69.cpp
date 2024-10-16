R_API void r_bin_java_print_deprecated_attr_summary(RBinJavaAttrInfo *attr) {
	if (!attr) {
		eprintf ("Attempting to print an invalid RBinJavaAttrInfo *Deperecated.\n");
		return;
	}
	Eprintf ("Deperecated Attribute Information:\n");
	Eprintf ("  Attribute Offset: 0x%08"PFMT64x "\n", attr->file_offset);
	Eprintf ("  Attribute Name Index: %d (%s)\n", attr->name_idx, attr->name);
	Eprintf ("  Attribute Length: %d\n", attr->length);
}