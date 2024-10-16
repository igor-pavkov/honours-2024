static int foreach_comment(void *user, const char *k, const char *v) {
	RAnalMetaUserItem *ui = user;
	RCore *core = ui->anal->user;
	const char *cmd = ui->user;
	if (!strncmp (k, "meta.C.", 7)) {
		char *cmt = (char *)sdb_decode (v, 0);
		if (cmt) {
			r_core_cmdf (core, "s %s", k + 7);
			r_core_cmd0 (core, cmd);
			free (cmt);
		}
	}
	return 1;
}