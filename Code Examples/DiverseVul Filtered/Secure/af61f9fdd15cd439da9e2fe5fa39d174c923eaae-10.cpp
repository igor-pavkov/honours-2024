void set_preferences_dir(std::string path)
{
#ifdef _WIN32
	if(path.empty()) {
		game_config::preferences_dir = get_cwd() + "/userdata";
	} else if (path.size() > 2 && path[1] == ':') {
		//allow absolute path override
		game_config::preferences_dir = path;
	} else {
		BOOL (*SHGetSpecialFolderPath)(HWND, LPTSTR, int, BOOL);
		HMODULE module = LoadLibrary("shell32");
		SHGetSpecialFolderPath = (BOOL (*)(HWND, LPTSTR, int, BOOL))GetProcAddress(module, "SHGetSpecialFolderPathA");
		if(SHGetSpecialFolderPath) {
			LOG_FS << "Using SHGetSpecialFolderPath to find My Documents\n";
			char my_documents_path[MAX_PATH];
			if(SHGetSpecialFolderPath(NULL, my_documents_path, 5, 1)) {
				std::string mygames_path = std::string(my_documents_path) + "/" + "My Games";
				boost::algorithm::replace_all(mygames_path, std::string("\\"), std::string("/"));
				create_directory_if_missing(mygames_path);
				game_config::preferences_dir = mygames_path + "/" + path;
			} else {
				WRN_FS << "SHGetSpecialFolderPath failed\n";
				game_config::preferences_dir = get_cwd() + "/" + path;
			}
		} else {
			LOG_FS << "Failed to load SHGetSpecialFolderPath function\n";
			game_config::preferences_dir = get_cwd() + "/" + path;
		}
	}

#else /*_WIN32*/

#ifdef PREFERENCES_DIR
	if (path.empty()) path = PREFERENCES_DIR;
#endif

	std::string path2 = ".wesnoth" + get_version_path_suffix();

#ifdef _X11
	const char *home_str = getenv("HOME");

	if (path.empty()) {
		char const *xdg_data = getenv("XDG_DATA_HOME");
		if (!xdg_data || xdg_data[0] == '\0') {
			if (!home_str) {
				path = path2;
				goto other;
			}
			user_data_dir = home_str;
			user_data_dir += "/.local/share";
		} else user_data_dir = xdg_data;
		user_data_dir += "/wesnoth/";
		user_data_dir += get_version_path_suffix();
		create_directory_if_missing_recursive(user_data_dir);
		game_config::preferences_dir = user_data_dir;
	} else {
		other:
		std::string home = home_str ? home_str : ".";

		if (path[0] == '/')
			game_config::preferences_dir = path;
		else
			game_config::preferences_dir = home + "/" + path;
	}
#else
	if (path.empty()) path = path2;

#ifdef __AMIGAOS4__
	game_config::preferences_dir = "PROGDIR:" + path;
#elif defined(__BEOS__)
	if (be_path.InitCheck() != B_OK) {
		BPath tpath;
		if (find_directory(B_USER_SETTINGS_DIRECTORY, &be_path, true) == B_OK) {
			be_path.Append("wesnoth");
		} else {
			be_path.SetTo("/boot/home/config/settings/wesnoth");
		}
		game_config::preferences_dir = be_path.Path();
	}
#else
	const char* home_str = getenv("HOME");
	std::string home = home_str ? home_str : ".";

	if (path[0] == '/')
		game_config::preferences_dir = path;
	else
		game_config::preferences_dir = home + std::string("/") + path;
#endif
#endif

#endif /*_WIN32*/
	user_data_dir = game_config::preferences_dir;
	setup_user_data_dir();
}