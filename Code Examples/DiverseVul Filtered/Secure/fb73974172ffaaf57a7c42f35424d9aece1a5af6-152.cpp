static void selinux_free_mnt_opts(void *mnt_opts)
{
	struct selinux_mnt_opts *opts = mnt_opts;
	kfree(opts->fscontext);
	kfree(opts->context);
	kfree(opts->rootcontext);
	kfree(opts->defcontext);
	kfree(opts);
}