R_API void r_bin_java_print_rtv_annotations_attr_summary(RBinJavaAttrInfo *attr) {
	if (attr && attr->type == R_BIN_JAVA_ATTR_TYPE_RUNTIME_VISIBLE_ANNOTATION_ATTR) {
		Eprintf ("Runtime Visible Annotations Attribute Information:\n");
		Eprintf ("   Attribute Offset: 0x%08"PFMT64x "\n", attr->file_offset);
		Eprintf ("   Attribute Name Index: %d (%s)\n", attr->name_idx, attr->name);
		Eprintf ("   Attribute Length: %d\n", attr->length);
		r_bin_java_print_annotation_array_summary (&attr->info.annotation_array);
	}
}