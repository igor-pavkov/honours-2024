static int lang_run_file(RCore *core, RLang *lang, const char *file) {
	r_core_sysenv_begin (core, NULL);
	return r_lang_run_file (core->lang, file);
}