R_API RBinJavaInterfaceInfo *r_bin_java_interface_new(RBinJavaObj *bin, const ut8 *buffer, ut64 sz) {
	RBinJavaInterfaceInfo *ifobj = NULL;
	ifobj = R_NEW0 (RBinJavaInterfaceInfo);
	IFDBG eprintf("Parsing RBinJavaInterfaceInfo\n");
	if (ifobj) {
		if (buffer) {
			ifobj->class_info_idx = R_BIN_JAVA_USHORT (buffer, 0);
			ifobj->cp_class = r_bin_java_get_item_from_bin_cp_list (bin, ifobj->class_info_idx);
			if (ifobj->cp_class) {
				ifobj->name = r_bin_java_get_item_name_from_bin_cp_list (bin, ifobj->cp_class);
			} else {
				ifobj->name = r_str_dup (NULL, "NULL");
			}
			ifobj->size = 2;
		} else {
			ifobj->class_info_idx = 0;
			ifobj->name = r_str_dup (NULL, "NULL");
		}
	}
	return ifobj;
}