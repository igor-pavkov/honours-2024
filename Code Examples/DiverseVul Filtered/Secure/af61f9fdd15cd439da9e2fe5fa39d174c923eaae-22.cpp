bool is_gzip_file(const std::string& filename)
{
	return (filename.length() > 3
		&& filename.substr(filename.length() - 3) == ".gz");
}