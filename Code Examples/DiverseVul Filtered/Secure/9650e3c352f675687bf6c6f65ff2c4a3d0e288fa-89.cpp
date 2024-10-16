R_API ut64 r_bin_java_read_class_file2(RBinJavaObj *bin, const ut64 offset, const ut8 *obuf, ut64 len) {
	const ut8 *cf2_buf = obuf + offset;
	RBinJavaCPTypeObj *this_class_cp_obj = NULL;
	IFDBG eprintf("\n0x%"PFMT64x " Offset before reading the cf2 structure\n", offset);
	/*
	Reading the following fields:
	ut16 access_flags;
	ut16 this_class;
	ut16 super_class;
	*/
	if (cf2_buf + 6 > obuf + len) {
		return 0;
	}
	bin->cf2.cf2_size = 6;
	bin->cf2.access_flags = R_BIN_JAVA_USHORT (cf2_buf, 0);
	bin->cf2.this_class = R_BIN_JAVA_USHORT (cf2_buf, 2);
	bin->cf2.super_class = R_BIN_JAVA_USHORT (cf2_buf, 4);
	free (bin->cf2.flags_str);
	free (bin->cf2.this_class_name);
	bin->cf2.flags_str = retrieve_class_method_access_string (bin->cf2.access_flags);
	this_class_cp_obj = r_bin_java_get_item_from_bin_cp_list (bin, bin->cf2.this_class);
	bin->cf2.this_class_name = r_bin_java_get_item_name_from_bin_cp_list (bin, this_class_cp_obj);
	IFDBG eprintf("This class flags are: %s\n", bin->cf2.flags_str);
	return bin->cf2.cf2_size;
}