void ftrace_set_global_notrace(unsigned char *buf, int len, int reset)
{
	ftrace_set_regex(&global_ops, buf, len, reset, 0);
}