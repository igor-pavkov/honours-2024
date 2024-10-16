std::string get_screenshot_dir()
{
	const std::string dir_path = get_user_data_dir() + "/screenshots";
	return get_dir(dir_path);
}