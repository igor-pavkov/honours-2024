static const std::string& get_version_path_suffix()
{
	static std::string suffix;

	// We only really need to generate this once since
	// the version number cannot change during runtime.

	if(suffix.empty()) {
		std::ostringstream s;
		s << game_config::wesnoth_version.major_version() << '.'
		  << game_config::wesnoth_version.minor_version();
		suffix = s.str();
	}

	return suffix;
}