const std::string &get_user_config_dir()
{
	if (user_config_dir.empty())
	{
#if defined(_X11) && !defined(PREFERENCES_DIR)
		char const *xdg_config = getenv("XDG_CONFIG_HOME");
		if (!xdg_config || xdg_config[0] == '\0') {
			xdg_config = getenv("HOME");
			if (!xdg_config) {
				user_config_dir = get_user_data_dir();
				return user_config_dir;
			}
			user_config_dir = xdg_config;
			user_config_dir += "/.config";
		} else user_config_dir = xdg_config;
		user_config_dir += "/wesnoth";
		create_directory_if_missing_recursive(user_config_dir);
#else
		user_config_dir = get_user_data_dir();
#endif
	}
	return user_config_dir;
}