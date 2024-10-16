void gf_sys_set_args(s32 argc, const char **argv)
{
	//for OSX we allow overwrite of argc/argv due to different behavior between console-mode apps and GUI
#if !defined(__DARWIN__) && !defined(__APPLE__)
	if (!gpac_argc && (argc>=0) )
#endif
	{
		gpac_argc = (u32) argc;
		gpac_argv = argv;
	}
}