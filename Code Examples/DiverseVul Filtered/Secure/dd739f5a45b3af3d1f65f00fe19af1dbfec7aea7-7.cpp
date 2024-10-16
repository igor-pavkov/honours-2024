static int cmd_panels(void *data, const char *input) {
	RCore *core = (RCore*) data;
	if (core->vmode) {
		return false;
	}
	if (*input == '?') {
		eprintf ("Usage: v[*i]\n");
		eprintf ("v.test    # save curren layout with name test\n");
		eprintf ("v test    # load saved layout with name test\n");
		eprintf ("vi ...    # launch 'vim'\n");
		return false;
	}
	if (*input == ' ') {
		if (core->panels) {
			r_load_panels_layout (core, input + 1);
		}
		r_config_set (core->config, "scr.layout", input + 1);
		return true;
	}
	if (*input == '=') {
		r_save_panels_layout (core, input + 1);
		r_config_set (core->config, "scr.layout", input + 1);
		return true;
	}
	if (*input == 'i') {
		r_sys_cmdf ("v%s", input);
		return false;
	}
	r_core_visual_panels_root (core, core->panels_root);
	return true;
}