const std::string& get_user_data_dir()
{
	// ensure setup gets called only once per session
	// FIXME: this is okay and optimized, but how should we react
	// if the user deletes a dir while we are running?
	if (user_data_dir.empty())
	{
		if (game_config::preferences_dir.empty())
			set_preferences_dir(std::string());
		else {
			user_data_dir = game_config::preferences_dir;
			setup_user_data_dir();
		}
	}
	return user_data_dir;
}