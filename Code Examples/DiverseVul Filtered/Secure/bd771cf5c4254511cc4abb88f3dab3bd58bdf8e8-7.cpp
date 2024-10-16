static void __init sm7xx_vga_setup(char *options)
{
	int i;

	if (!options || !*options)
		return;

	smtc_scr_info.lfb_width = 0;
	smtc_scr_info.lfb_height = 0;
	smtc_scr_info.lfb_depth = 0;

	pr_debug("%s = %s\n", __func__, options);

	for (i = 0; i < ARRAY_SIZE(vesa_mode_table); i++) {
		if (strstr(options, vesa_mode_table[i].index)) {
			smtc_scr_info.lfb_width  = vesa_mode_table[i].lfb_width;
			smtc_scr_info.lfb_height =
						vesa_mode_table[i].lfb_height;
			smtc_scr_info.lfb_depth  = vesa_mode_table[i].lfb_depth;
			return;
		}
	}
}