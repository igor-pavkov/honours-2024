R_API void r_bin_java_print_enclosing_methods_attr_summary(RBinJavaAttrInfo *attr) {
	if (!attr) {
		eprintf ("Attempting to print an invalid RBinJavaAttrInfo *Deperecated.\n");
		return;
	}
	Eprintf ("Enclosing Method Attribute Information:\n");
	Eprintf ("  Attribute Offset: 0x%08"PFMT64x "\n", attr->file_offset);
	Eprintf ("  Attribute Name Index: %d (%s)\n", attr->name_idx, attr->name);
	Eprintf ("  Attribute Length: %d\n", attr->length);
	Eprintf ("  Class Info Index : 0x%02x\n", attr->info.enclosing_method_attr.class_idx);
	Eprintf ("  Method Name and Type Index : 0x%02x\n", attr->info.enclosing_method_attr.method_idx);
	Eprintf ("  Class Name : %s\n", attr->info.enclosing_method_attr.class_name);
	Eprintf ("  Method Name and Desc : %s %s\n", attr->info.enclosing_method_attr.method_name, attr->info.enclosing_method_attr.method_descriptor);
}