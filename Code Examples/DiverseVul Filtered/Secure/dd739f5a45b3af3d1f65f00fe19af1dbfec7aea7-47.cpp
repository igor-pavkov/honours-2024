static bool set_tmp_arch(RCore *core, char *arch, char **tmparch) {
	if (!tmparch) {
		eprintf ("tmparch should be set\n");
	}
	*tmparch = strdup (r_config_get (core->config, "asm.arch"));
	r_config_set (core->config, "asm.arch", arch);
	core->fixedarch = true;
	return true;
}