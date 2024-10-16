static int g_show(struct seq_file *m, void *v)
{
	unsigned long *ptr = v;

	if (!ptr)
		return 0;

	if (ptr == (unsigned long *)1) {
		seq_printf(m, "#### all functions enabled ####\n");
		return 0;
	}

	seq_printf(m, "%ps\n", (void *)*ptr);

	return 0;
}