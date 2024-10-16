static int selinux_fs_context_dup(struct fs_context *fc,
				  struct fs_context *src_fc)
{
	const struct selinux_mnt_opts *src = src_fc->security;
	struct selinux_mnt_opts *opts;

	if (!src)
		return 0;

	fc->security = kzalloc(sizeof(struct selinux_mnt_opts), GFP_KERNEL);
	if (!fc->security)
		return -ENOMEM;

	opts = fc->security;

	if (src->fscontext) {
		opts->fscontext = kstrdup(src->fscontext, GFP_KERNEL);
		if (!opts->fscontext)
			return -ENOMEM;
	}
	if (src->context) {
		opts->context = kstrdup(src->context, GFP_KERNEL);
		if (!opts->context)
			return -ENOMEM;
	}
	if (src->rootcontext) {
		opts->rootcontext = kstrdup(src->rootcontext, GFP_KERNEL);
		if (!opts->rootcontext)
			return -ENOMEM;
	}
	if (src->defcontext) {
		opts->defcontext = kstrdup(src->defcontext, GFP_KERNEL);
		if (!opts->defcontext)
			return -ENOMEM;
	}
	return 0;
}