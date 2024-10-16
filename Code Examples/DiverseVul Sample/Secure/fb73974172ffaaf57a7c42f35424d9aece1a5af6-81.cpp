static int show_sid(struct seq_file *m, u32 sid)
{
	char *context = NULL;
	u32 len;
	int rc;

	rc = security_sid_to_context(&selinux_state, sid,
					     &context, &len);
	if (!rc) {
		bool has_comma = context && strchr(context, ',');

		seq_putc(m, '=');
		if (has_comma)
			seq_putc(m, '\"');
		seq_escape(m, context, "\"\n\\");
		if (has_comma)
			seq_putc(m, '\"');
	}
	kfree(context);
	return rc;
}