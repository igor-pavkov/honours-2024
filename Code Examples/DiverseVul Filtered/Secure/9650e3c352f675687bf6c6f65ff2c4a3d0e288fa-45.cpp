R_API ut64 r_bin_java_parse_interfaces(RBinJavaObj *bin, const ut64 offset, const ut8 *buf, const ut64 len) {
	int i = 0;
	ut64 adv = 0;
	RBinJavaInterfaceInfo *interfaces_obj;
	const ut8 *if_buf = buf + offset;
	bin->cp_offset = offset;
	bin->interfaces_offset = offset;
	r_list_free (bin->interfaces_list);
	bin->interfaces_list = r_list_newf (r_bin_java_interface_free);
	if (offset + 2 > len) {
		bin->interfaces_size = 0;
		return 0;
	}
	bin->interfaces_count = R_BIN_JAVA_USHORT (if_buf, 0);
	adv += 2;
	IFDBG eprintf("Interfaces count: %d\n", bin->interfaces_count);
	if (bin->interfaces_count > 0) {
		for (i = 0; i < bin->interfaces_count; i++) {
			interfaces_obj = r_bin_java_read_next_interface_item (bin, offset + adv, buf, len);
			if (interfaces_obj) {
				r_list_append (bin->interfaces_list, interfaces_obj);
				adv += interfaces_obj->size;
				if (offset + adv > len) {
					eprintf ("[X] r_bin_java: Error unable to parse remainder of classfile after Interface: %d.\n", i);
					break;
				}
			} else {
				break;
			}
		}
	}
	bin->interfaces_size = adv;
	return adv;
}