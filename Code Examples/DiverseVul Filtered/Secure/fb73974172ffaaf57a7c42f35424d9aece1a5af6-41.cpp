static int selinux_add_mnt_opt(const char *option, const char *val, int len,
			       void **mnt_opts)
{
	int token = Opt_error;
	int rc, i;

	for (i = 0; i < ARRAY_SIZE(tokens); i++) {
		if (strcmp(option, tokens[i].name) == 0) {
			token = tokens[i].opt;
			break;
		}
	}

	if (token == Opt_error)
		return -EINVAL;

	if (token != Opt_seclabel) {
		val = kmemdup_nul(val, len, GFP_KERNEL);
		if (!val) {
			rc = -ENOMEM;
			goto free_opt;
		}
	}
	rc = selinux_add_opt(token, val, mnt_opts);
	if (unlikely(rc)) {
		kfree(val);
		goto free_opt;
	}
	return rc;

free_opt:
	if (*mnt_opts) {
		selinux_free_mnt_opts(*mnt_opts);
		*mnt_opts = NULL;
	}
	return rc;
}