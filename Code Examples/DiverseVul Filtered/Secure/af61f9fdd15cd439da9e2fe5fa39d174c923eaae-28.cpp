std::string get_intl_dir()
{
#ifdef _WIN32
	return get_cwd() + "/translations";
#else

#ifdef USE_INTERNAL_DATA
	return get_cwd() + "/" LOCALEDIR;
#endif

#if HAS_RELATIVE_LOCALEDIR
	std::string res = game_config::path + "/" LOCALEDIR;
#else
	std::string res = LOCALEDIR;
#endif

	return res;
#endif
}