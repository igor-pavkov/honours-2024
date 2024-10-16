R_API REgg *r_egg_new() {
	int i;
	REgg *egg = R_NEW0 (REgg);
	if (!egg) {
		return NULL;
	}
	egg->src = r_buf_new ();
	if (!egg->src) {
		goto beach;
	}
	egg->buf = r_buf_new ();
	if (!egg->buf) {
		goto beach;
	}
	egg->bin = r_buf_new ();
	if (!egg->bin) {
		goto beach;
	}
	egg->remit = &emit_x86;
	egg->syscall = r_syscall_new ();
	if (!egg->syscall) {
		goto beach;
	}
	egg->rasm = r_asm_new ();
	if (!egg->rasm) {
		goto beach;
	}
	egg->bits = 0;
	egg->endian = 0;
	egg->db = sdb_new (NULL, NULL, 0);
	if (!egg->db) {
		goto beach;
	}
	egg->patches = r_list_newf (egg_patch_free);
	if (!egg->patches) {
		goto beach;
	}
	egg->plugins = r_list_new ();
	for (i=0; egg_static_plugins[i]; i++) {
		r_egg_add (egg, egg_static_plugins[i]);
	}
	return egg;

beach:
	r_egg_free (egg);
	return NULL;
}