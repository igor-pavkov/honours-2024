std::string read_file(const std::string &fname)
{
	scoped_istream s = istream_file(fname);
	return read_stream(*s);
}