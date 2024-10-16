static int __init set_ftrace_filter(char *str)
{
	strncpy(ftrace_filter_buf, str, FTRACE_FILTER_SIZE);
	return 1;
}