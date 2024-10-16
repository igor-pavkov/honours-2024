static int selinux_add_opt(int token, const char *s, void **mnt_opts)
{
	struct selinux_mnt_opts *opts = *mnt_opts;

	if (token == Opt_seclabel)	/* eaten and completely ignored */
		return 0;

	if (!opts) {
		opts = kzalloc(sizeof(struct selinux_mnt_opts), GFP_KERNEL);
		if (!opts)
			return -ENOMEM;
		*mnt_opts = opts;
	}
	if (!s)
		return -ENOMEM;
	switch (token) {
	case Opt_context:
		if (opts->context || opts->defcontext)
			goto Einval;
		opts->context = s;
		break;
	case Opt_fscontext:
		if (opts->fscontext)
			goto Einval;
		opts->fscontext = s;
		break;
	case Opt_rootcontext:
		if (opts->rootcontext)
			goto Einval;
		opts->rootcontext = s;
		break;
	case Opt_defcontext:
		if (opts->context || opts->defcontext)
			goto Einval;
		opts->defcontext = s;
		break;
	}
	return 0;
Einval:
	pr_warn(SEL_MOUNT_FAIL_MSG);
	return -EINVAL;
}