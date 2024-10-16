std::string get_program_invocation(const std::string& program_name) {
#ifdef DEBUG
  #ifdef _WIN32
	const char *program_suffix = "-debug.exe";
  #else
	const char *program_suffix = "-debug";
  #endif
#else
  #ifdef _WIN32
	const char *program_suffix = ".exe";
  #else
	const char *program_suffix = "";
  #endif
#endif

	const std::string real_program_name(program_name + program_suffix);
	if(game_config::wesnoth_program_dir.empty()) return real_program_name;
#ifdef _WIN32
	return game_config::wesnoth_program_dir + "\\" + real_program_name;
#else
	return game_config::wesnoth_program_dir + "/" + real_program_name;
#endif
}