bool file_exists(const std::string& name)
{
#ifdef _WIN32
       struct stat st;
       return (::stat(name.c_str(), &st) == 0);
#else
	struct stat st;
	return (::stat(name.c_str(), &st) != -1);
#endif
}