std::string get_addon_campaigns_dir()
{
	const std::string dir_path = get_user_data_dir() + "/data/add-ons";
	return get_dir(dir_path);
}