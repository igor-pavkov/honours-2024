void PrintCoreUsage()
{
	gf_sys_format_help(helpout, help_flags, "# libgpac core options\n");
	gf_sys_print_core_help(helpout, 0, GF_ARGMODE_ALL, 0);
}