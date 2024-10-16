R_API void r_bin_java_print_line_number_attr_summary(RBinJavaLineNumberAttribute *lnattr) {
	if (!lnattr) {
		eprintf ("Attempting to print an invalid RBinJavaLineNumberAttribute *.\n");
		return;
	}
	Eprintf ("  Line Number Attribute Offset: 0x%08"PFMT64x "\n", lnattr->file_offset);
	Eprintf ("  Line Number Attribute StartPC: %d\n", lnattr->start_pc);
	Eprintf ("  Line Number Attribute LineNumber: %d\n", lnattr->line_number);
}