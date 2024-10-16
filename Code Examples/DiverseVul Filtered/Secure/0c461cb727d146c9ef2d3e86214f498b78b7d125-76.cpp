static void selinux_write_opts(struct seq_file *m,
			       struct security_mnt_opts *opts)
{
	int i;
	char *prefix;

	for (i = 0; i < opts->num_mnt_opts; i++) {
		char *has_comma;

		if (opts->mnt_opts[i])
			has_comma = strchr(opts->mnt_opts[i], ',');
		else
			has_comma = NULL;

		switch (opts->mnt_opts_flags[i]) {
		case CONTEXT_MNT:
			prefix = CONTEXT_STR;
			break;
		case FSCONTEXT_MNT:
			prefix = FSCONTEXT_STR;
			break;
		case ROOTCONTEXT_MNT:
			prefix = ROOTCONTEXT_STR;
			break;
		case DEFCONTEXT_MNT:
			prefix = DEFCONTEXT_STR;
			break;
		case SBLABEL_MNT:
			seq_putc(m, ',');
			seq_puts(m, LABELSUPP_STR);
			continue;
		default:
			BUG();
			return;
		};
		/* we need a comma before each option */
		seq_putc(m, ',');
		seq_puts(m, prefix);
		if (has_comma)
			seq_putc(m, '\"');
		seq_escape(m, opts->mnt_opts[i], "\"\n\\");
		if (has_comma)
			seq_putc(m, '\"');
	}
}