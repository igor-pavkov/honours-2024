bool create_directory_if_missing_recursive(const std::string& dirname)
{
	DBG_FS<<"creating recursive directory: "<<dirname<<'\n';
	if (is_directory(dirname) == false && dirname.empty() == false)
	{
		std::string tmp_dirname = dirname;
		// remove trailing slashes or backslashes
		while ((tmp_dirname[tmp_dirname.size()-1] == '/' ||
			  tmp_dirname[tmp_dirname.size()-1] == '\\') &&
			  !tmp_dirname.empty())
		{
			tmp_dirname.erase(tmp_dirname.size()-1);
		}

		// create the first non-existing directory
		size_t pos = tmp_dirname.rfind("/");

		// we get the most right directory and *skip* it
		// we are creating it when we get back here
		if (tmp_dirname.rfind('\\') != std::string::npos &&
			tmp_dirname.rfind('\\') > pos )
			pos = tmp_dirname.rfind('\\');

		if (pos != std::string::npos)
			create_directory_if_missing_recursive(tmp_dirname.substr(0,pos));

		return create_directory_if_missing(tmp_dirname);
	}
	return create_directory_if_missing(dirname);
}