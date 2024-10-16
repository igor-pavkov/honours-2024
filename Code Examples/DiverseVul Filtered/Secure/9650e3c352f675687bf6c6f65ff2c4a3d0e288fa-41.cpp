R_API void r_bin_java_print_source_debug_attr_summary(RBinJavaAttrInfo *attr) {
	ut32 i = 0;
	if (attr == NULL) {
		eprintf ("Attempting to print an invalid RBinJavaSourceDebugExtensionAttr *.\n");
		return;
	}
	Eprintf ("Source Debug Extension Attribute Information:\n");
	Eprintf ("  Attribute Offset: 0x%08"PFMT64x "\n", attr->file_offset);
	Eprintf ("  Attribute Name Index: %d (%s)\n", attr->name_idx, attr->name);
	Eprintf ("  Extension Length: %d\n", attr->length);
	Eprintf ("  Source Debug Extension value: \n");
	for (i = 0; i < attr->length; i++) {
		Eprintf ("%c", attr->info.debug_extensions.debug_extension[i]);
	}
	Eprintf ("\n  Source Debug Extension End\n");
}