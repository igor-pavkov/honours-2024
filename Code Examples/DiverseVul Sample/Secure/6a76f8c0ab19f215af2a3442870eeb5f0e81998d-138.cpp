static int function_stat_headers(struct seq_file *m)
{
#ifdef CONFIG_FUNCTION_GRAPH_TRACER
	seq_printf(m, "  Function                               "
		   "Hit    Time            Avg             s^2\n"
		      "  --------                               "
		   "---    ----            ---             ---\n");
#else
	seq_printf(m, "  Function                               Hit\n"
		      "  --------                               ---\n");
#endif
	return 0;
}