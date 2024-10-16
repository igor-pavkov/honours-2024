R_API void r_bin_java_print_signature_attr_summary(RBinJavaAttrInfo *attr) {
	if (!attr) {
		eprintf ("Attempting to print an invalid RBinJavaAttrInfo *SignatureAttr.\n");
		return;
	}
	Eprintf ("Signature Attribute Information:\n");
	Eprintf ("  Attribute Offset: 0x%08"PFMT64x "\n", attr->file_offset);
	Eprintf ("  Attribute Name Index: %d (%s)\n", attr->name_idx, attr->name);
	Eprintf ("  Attribute Length: %d\n", attr->length);
	Eprintf ("  Signature UTF8 Index: %d\n", attr->info.signature_attr.signature_idx);
	Eprintf ("  Signature string: %s\n", attr->info.signature_attr.signature);
}