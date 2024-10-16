static int __init ftrace_mod_cmd_init(void)
{
	return register_ftrace_command(&ftrace_mod_cmd);
}