static int __init set_graph_function(char *str)
{
	strlcpy(ftrace_graph_buf, str, FTRACE_FILTER_SIZE);
	return 1;
}