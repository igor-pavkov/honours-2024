bool is_directory(const std::string& fname)
{
	if(fname.empty()) {
		return false;
	}
	if(fname[0] != '/' && !game_config::path.empty()) {
		if(is_directory_internal(game_config::path + "/" + fname))
			return true;
	}

	return is_directory_internal(fname);
}