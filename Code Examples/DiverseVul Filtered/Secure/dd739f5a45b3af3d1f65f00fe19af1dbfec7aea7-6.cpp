static bool set_tmp_bits(RCore *core, int bits, char **tmpbits) {
	if (!tmpbits) {
		eprintf ("tmpbits should be set\n");
	}
	*tmpbits = strdup (r_config_get (core->config, "asm.bits"));
	r_config_set_i (core->config, "asm.bits", bits);
	core->fixedbits = true;
	return true;
}