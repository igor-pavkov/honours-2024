void unregister_ftrace_function_probe_all(char *glob)
{
	__unregister_ftrace_function_probe(glob, NULL, NULL, 0);
}