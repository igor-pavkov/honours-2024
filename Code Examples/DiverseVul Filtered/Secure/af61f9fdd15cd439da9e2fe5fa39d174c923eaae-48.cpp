bool make_directory(const std::string& path)
{
	return (mkdir(path.c_str(),AccessMode) == 0);
}