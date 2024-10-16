R_API int r_bin_java_load_bin(RBinJavaObj *bin, const ut8 *buf, ut64 buf_sz) {
	ut64 adv = 0;
	R_BIN_JAVA_GLOBAL_BIN = bin;
	if (!bin) {
		return false;
	}
	r_bin_java_reset_bin_info (bin);
	memcpy ((ut8 *) &bin->cf, buf, 10);
	if (memcmp (bin->cf.cafebabe, "\xCA\xFE\xBA\xBE", 4)) {
		eprintf ("r_bin_java_new_bin: Invalid header (%02x %02x %02x %02x)\n",
			bin->cf.cafebabe[0], bin->cf.cafebabe[1],
			bin->cf.cafebabe[2], bin->cf.cafebabe[3]);
		return false;
	}
	if (bin->cf.major[0] == bin->cf.major[1] && bin->cf.major[0] == 0) {
		eprintf ("Java CLASS with MACH0 header?\n");
		return false;
	}
	adv += 8;
	// -2 so that the cp_count will be parsed
	adv += r_bin_java_parse_cp_pool (bin, adv, buf, buf_sz);
	if (adv > buf_sz) {
		eprintf ("[X] r_bin_java: Error unable to parse remainder of classfile after Constant Pool.\n");
		return true;
	}
	adv += r_bin_java_read_class_file2 (bin, adv, buf, buf_sz);
	if (adv > buf_sz) {
		eprintf ("[X] r_bin_java: Error unable to parse remainder of classfile after class file info.\n");
		return true;
	}
	IFDBG eprintf("This class: %d %s\n", bin->cf2.this_class, bin->cf2.this_class_name);
	IFDBG eprintf("0x%"PFMT64x " Access flags: 0x%04x\n", adv, bin->cf2.access_flags);
	adv += r_bin_java_parse_interfaces (bin, adv, buf, buf_sz);
	if (adv > buf_sz) {
		eprintf ("[X] r_bin_java: Error unable to parse remainder of classfile after Interfaces.\n");
		return true;
	}
	adv += r_bin_java_parse_fields (bin, adv, buf, buf_sz);
	if (adv > buf_sz) {
		eprintf ("[X] r_bin_java: Error unable to parse remainder of classfile after Fields.\n");
		return true;
	}
	adv += r_bin_java_parse_methods (bin, adv, buf, buf_sz);
	if (adv > buf_sz) {
		eprintf ("[X] r_bin_java: Error unable to parse remainder of classfile after Methods.\n");
		return true;
	}
	adv += r_bin_java_parse_attrs (bin, adv, buf, buf_sz);
	bin->calc_size = adv;
	// if (adv > buf_sz) {
	// eprintf ("[X] r_bin_java: Error unable to parse remainder of classfile after Attributes.\n");
	// return true;
	// }

	// add_cp_objs_to_sdb(bin);
	// add_method_infos_to_sdb(bin);
	// add_field_infos_to_sdb(bin);
	return true;
}