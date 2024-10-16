static int __ftrace_modify_code(void *data)
{
	int *command = data;

	ftrace_modify_all_code(*command);

	return 0;
}