R_API void r_bin_java_print_inner_classes_attr_summary(RBinJavaAttrInfo *attr) {
	RBinJavaClassesAttribute *icattr;
	RListIter *iter, *iter_tmp;
	if (!attr) {
		eprintf ("Attempting to print an invalid RBinJavaAttrInfo *InnerClasses.\n");
		return;
	}
	Eprintf ("Inner Classes Attribute Information:\n");
	Eprintf ("  Attribute Offset: 0x%08"PFMT64x "\n", attr->file_offset);
	Eprintf ("  Attribute Name Index: %d (%s)\n", attr->name_idx, attr->name);
	Eprintf ("  Attribute Length: %d\n", attr->length);
	r_list_foreach_safe (attr->info.inner_classes_attr.classes, iter, iter_tmp, icattr) {
		r_bin_java_print_classes_attr_summary (icattr);
	}
}