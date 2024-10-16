void PrintCICP()
{
	u32 i;
	gf_sys_format_help(helpout, help_flags, "# Video CICP (ISO/IEC 23091-2) Constants\n");
	gf_sys_format_help(helpout, help_flags, "CICP Color Primaries:\n");
	for (i=0; i<GF_CICP_PRIM_LAST; i++) {
		const char *name = gf_cicp_color_primaries_name(i);
		if (!name || !strcmp(name, "unknwon")) continue;
		gf_sys_format_help(helpout, help_flags, " - `%s` (value %d)\n", name, i);
	}
	gf_sys_format_help(helpout, help_flags, "  \nCICP Color Transfer Characteristics:\n");
	for (i=0; i<GF_CICP_TRANSFER_LAST; i++) {
		const char *name = gf_cicp_color_transfer_name(i);
		if (!name) continue;
		gf_sys_format_help(helpout, help_flags, " - `%s` (value %d)\n", name, i);
	}
	gf_sys_format_help(helpout, help_flags, "  \nCICP Color Matrix Coefficients:\n");
	for (i=0; i<GF_CICP_MX_LAST; i++) {
		const char *name = gf_cicp_color_matrix_name(i);
		if (!name) continue;
		gf_sys_format_help(helpout, help_flags, " - `%s` (value %d)\n", name, i);
	}
}