R_API RBinJavaCPTypeObj *r_bin_java_find_cp_name_and_type_info(RBinJavaObj *bin, ut16 name_idx, ut16 descriptor_idx) {
	RListIter *iter, *iter_tmp;
	RBinJavaCPTypeObj *res = NULL, *obj = NULL;
	IFDBG eprintf("Looking for name_idx: %d and descriptor_idx: %d\n", name_idx, descriptor_idx);
	r_list_foreach_safe (bin->cp_list, iter, iter_tmp, obj) {
		if (obj && obj->tag == R_BIN_JAVA_CP_NAMEANDTYPE) {
			IFDBG eprintf("RBinJavaCPTypeNameAndType has name_idx: %d and descriptor_idx: %d\n",
			obj->info.cp_name_and_type.name_idx, obj->info.cp_name_and_type.descriptor_idx);
			if (obj->info.cp_name_and_type.name_idx == name_idx &&
			obj->info.cp_name_and_type.descriptor_idx == descriptor_idx) {
				res = obj;
				break;
			}
		}
	}
	return res;
}