static void header(RBinFile *arch) {
	struct r_bin_dex_obj_t *bin = arch->o->bin_obj;
	struct r_bin_t *rbin = arch->rbin;

	rbin->cb_printf ("DEX file header:\n");
	rbin->cb_printf ("magic               : 'dex\\n035\\0'\n");
	rbin->cb_printf ("checksum            : %x\n", bin->header.checksum);
	rbin->cb_printf ("signature           : %02x%02x...%02x%02x\n", bin->header.signature[0], bin->header.signature[1], bin->header.signature[18], bin->header.signature[19]);
	rbin->cb_printf ("file_size           : %d\n", bin->header.size);
	rbin->cb_printf ("header_size         : %d\n", bin->header.header_size);
	rbin->cb_printf ("link_size           : %d\n", bin->header.linksection_size);
	rbin->cb_printf ("link_off            : %d (0x%06x)\n", bin->header.linksection_offset, bin->header.linksection_offset);
	rbin->cb_printf ("string_ids_size     : %d\n", bin->header.strings_size);
	rbin->cb_printf ("string_ids_off      : %d (0x%06x)\n", bin->header.strings_offset, bin->header.strings_offset);
	rbin->cb_printf ("type_ids_size       : %d\n", bin->header.types_size);
	rbin->cb_printf ("type_ids_off        : %d (0x%06x)\n", bin->header.types_offset, bin->header.types_offset);
	rbin->cb_printf ("proto_ids_size       : %d\n", bin->header.prototypes_size);
	rbin->cb_printf ("proto_ids_off        : %d (0x%06x)\n", bin->header.prototypes_offset, bin->header.prototypes_offset);
	rbin->cb_printf ("field_ids_size      : %d\n", bin->header.fields_size);
	rbin->cb_printf ("field_ids_off       : %d (0x%06x)\n", bin->header.fields_offset, bin->header.fields_offset);
	rbin->cb_printf ("method_ids_size     : %d\n", bin->header.method_size);
	rbin->cb_printf ("method_ids_off      : %d (0x%06x)\n", bin->header.method_offset, bin->header.method_offset);
	rbin->cb_printf ("class_defs_size     : %d\n", bin->header.class_size);
	rbin->cb_printf ("class_defs_off      : %d (0x%06x)\n", bin->header.class_offset, bin->header.class_offset);
	rbin->cb_printf ("data_size           : %d\n", bin->header.data_size);
	rbin->cb_printf ("data_off            : %d (0x%06x)\n\n", bin->header.data_offset, bin->header.data_offset);

	// TODO: print information stored in the RBIN not this ugly fix
	dexdump = true;
	bin->methods_list = NULL;
	dex_loadcode (arch, bin);
	dexdump = false;
}