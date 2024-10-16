static int __init set_ftrace_notrace(char *str)
{
	strncpy(ftrace_notrace_buf, str, FTRACE_FILTER_SIZE);
	return 1;
}