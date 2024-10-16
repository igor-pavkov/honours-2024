const char *gf_sys_get_arg(u32 arg)
{
	if (!gpac_argc || !gpac_argv) return NULL;
	if (arg>=gpac_argc) return NULL;
	return gpac_argv[arg];
}